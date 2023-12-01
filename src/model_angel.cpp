#include "model_angel.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "debug.h"

#include "window_demon.h"

#include "net_common.h"

const char* slabstat_ids[] = {
	"sock",
	"tcp_bind_bucket",
	"tcp_tw_bucket",
	"tcp_open_request",
	"skbuff_head_cache",
};

static FILE* generic_proc_open(const char* env, const char* name){
	const char* p = getenv(env);
	char store[128];

	if(!p){
		p = getenv("PROC_ROOT") ? : "/proc";
		snprintf(store, sizeof(store)-1, "%s/%s", p, name);
		p = store;
	}

	return fopen(p, "r");
}

static FILE* net_tcp_open(){
	return generic_proc_open("PROC_NET_TCP", "net/tcp");
}

static FILE* net_tcp6_open(){
	return generic_proc_open("PROC_NET_TCP6", "net/tcp6");
}

static FILE* net_udp_open(){
	return generic_proc_open("PROC_NET_UDP", "net/udp");
}

static FILE* net_udp6_open(){
	return generic_proc_open("PROC_NET_UDP6", "net/udp6");
}

static FILE* slabinfo_open(){
	return generic_proc_open("PROC_SLAB_INFO", "slabinfo");
}

int get_slabstat(struct slabstat* s){
	char buf[256];
	FILE* fp;
	int count;

	memset(s, 0, sizeof(struct slabstat));

	fp = slabinfo_open();
	if(!fp){
		dbgprint("Could not open /proc/slabinfo.\n");
		return -1;
	}

	count = sizeof(struct slabstat) / sizeof(int);

	fgets(buf, sizeof(buf), fp);
	while(fgets(buf, sizeof(buf), fp) != NULL){
		for(int i = 0; i < sizeof(slabstat_ids[i]) / sizeof(slabstat_ids[0]); i++){
			if(memcmp(buf, slabstat_ids[i], strlen(slabstat_ids[i])) == 0){
				sscanf(buf, "%*s%d", ((int *)s) + i);

				count--;
				break;
			}
		}

		if(count <= 0){
			break;
		}
	}

	return 0;
}

static void user_ent_hash_build();

model_angel::model_angel(){
	//Ignore the error!
	get_slabstat(&slabstat);

	//Only if you want to show process info.
	user_ent_hash_build();

	num_warnings = 0;
	num_alerts = 0;
}

int model_angel::start(window_demon* pointer_to_demon){
	demon_pointer = pointer_to_demon;

	int r = net_agent.init();
	if(r == -1){
		dbgprint("Failed to start networking agent!\n");
		return -1;
	}

	return 0;
}

model_angel::~model_angel(){
	for(message* p : message_log){
		free(p->text);
		free(p);
	}
}

void model_angel::warn(){
	this->num_warnings++;
}

void model_angel::alert(const char* alarum_message){
	if(this->num_alerts == 0){
		demon_pointer->trigger_alarum_popup(alarum_message);
	}
	this->num_alerts++;
}

void model_angel::reset_alerts_and_warns(){
	this->num_warnings = 0;
	this->num_alerts = 0;
}

std::vector<conn_entry*> model_angel::get_connections(){
	return this->connections;
}

std::vector<message*> model_angel::get_log(){
	return this->message_log;
}

unsigned int model_angel::get_num_connections(){
	return this->connections.size();
}

int model_angel::get_num_warnings(){
	return this->num_warnings;
}

int model_angel::get_num_alerts(){
	return this->num_alerts;
}

int model_angel::get_good_buffer(char** buf, int* bufsize){
	//Estimate amount of sockets and try to allocate
	//huge buffer to read all the table with one read.
	//Limit it to 16MB, though. The assumption is: as soon as
	//the kernel is able to hold information about N connections,
	//it is able to give us some memory for a snapshot.
	if(1){
		int guess = slabstat.socks + slabstat.tcp_syns;

		if(guess > (16*1024*1024)/128){
			guess = (16*1024*1024)/128;
		}
		guess *= 128;

		if(guess > *bufsize){
			*bufsize = guess;
		}
	}

	while(*bufsize >= 64*1024){
		if((*buf = (char*)malloc(*bufsize)) != NULL){
			break;
		}

		*bufsize /= 2;
	}
	if(*buf == NULL){
		dbgprint("Could not allocate memory for a buffer.\n");
		errno = ENOMEM;
		return -1;
	}

	return 0;
}

enum{
	TCP,
	UDP,
};

int model_angel::open_generic_proc(FILE* fp, int AF, int netid){
	char line[256];

	dbgprint("[MODEL_ANGEL] Reading a /proc file...\n");

	//Skip header in /proc/whatever.
	if(fgets(line, sizeof(line), fp) == NULL){
		dbgprint("[MODEL_ANGEL] Could not read header of a /proc file.\n");
		goto outerr;
	}

	while(fgets(line, sizeof(line), fp) != NULL){
		int n = strlen(line);
		if(n == 0 || line[n-1] != '\n'){
			dbgprint("[MODEL_ANGEL] Failed to parse a line in a /proc file.\n");
			errno = EINVAL;
			return -1;
		}
		line[n-1] = '\0';

		switch(netid){
			case TCP:
				if(tcp_parse_proc_line(line, AF) < 0){
					return 0;
				}
				break;
			case UDP:
				if(udp_parse_proc_line(line, AF) < 0){
					return 0;
				}
				break;
		}
	}

outerr:
	return ferror(fp) ? -1 : 0;
}

int model_angel::open_proc_tcp(FILE* fp, int AF){
	return open_generic_proc(fp, AF, TCP);
}

int model_angel::open_proc_udp(FILE* fp, int AF){
	return open_generic_proc(fp, AF, UDP);
}

//Include for __u8, etc.
#include <asm/types.h>

typedef struct {
	__u8 family;
	__u8 bytelen;
	__s16 bitlen;
	__u32 flags;
	__u32 data[8];
} inet_prefix;

struct tcpstat {
	inet_prefix local;
	inet_prefix remote;
	int			local_port;
	int			remote_port;
	int			state;
	int			rq, wq;
	int			timer;
	int 		timeout;
	int			retrs;
	unsigned	ino;
	int			probes;
	unsigned	uid;
	int			refcnt;
	unsigned long long sk;
	int			rto, ato, qack, cwnd, ssthresh;
};

static const char* sstate_name[] = {
	"UNKNOWN",
	[TCP_ESTABLISHED] = "ESTAB",
	[TCP_SYN_SENT] = "SYN-SENT",
	[TCP_SYN_RECV] = "SYN-RECV",
	[TCP_FIN_WAIT1] = "FIN-WAIT-1",
	[TCP_FIN_WAIT2] = "FIN-WAIT-2",
	[TCP_TIME_WAIT] = "TIME-WAIT",
	[TCP_CLOSE] = "UNCONN",
	[TCP_CLOSE_WAIT] = "CLOSE-WAIT",
	[TCP_LAST_ACK] = "LAST-ACK",
	[TCP_LISTEN] = "LISTEN",
	[TCP_CLOSING] = "CLOSING",
};

struct sockaddr* inet_prefix_to_sockaddr(const inet_prefix* addr, unsigned int* len, int port){
	struct sockaddr* sa;

	struct sockaddr_in* sin;
	struct sockaddr_in6* sin6;

dbgprint("[DEBUG] Entering inet_prefix_to_sockaddr...\n");

	//IPv4
	if(addr->family == AF_INET){
dbgprint("[DEBUG] IP is v4...\n");
		sin = (struct sockaddr_in*) calloc(1, sizeof(struct sockaddr_in));

		sin->sin_family = AF_INET;
dbgprint("[DEBUG] port is: %08x\n", port);
dbgprint("[DEBUG] sin_port is: %08x\n", port);
		sin->sin_port = port;
		memcpy(&(sin->sin_addr), addr->data, 4 * 1);
dbgprint("[DEBUG] addr->data is: %08x\n", *addr->data);

		sa = (struct sockaddr*) sin;

		*len = sizeof(struct sockaddr_in);
	}
	//IPv6
	else{
dbgprint("[DEBUG] IP is v6...\n");
		sin6 = (struct sockaddr_in6*) calloc(1, sizeof(struct sockaddr_in6));

		sin6->sin6_family = addr->family;
dbgprint("[DEBUG] port is: %08x\n", port);
dbgprint("[DEBUG] sin_port is: %08x\n", port);
		sin6->sin6_port = port;
		memcpy(&(sin6->sin6_addr), addr->data, 4 * 4);
dbgprint("[DEBUG] addr->data is: %032x\n", *addr->data);

		sa = (struct sockaddr*) sin6;

		*len = sizeof(struct sockaddr_in6);
	}

dbgprint("[DEBUG] sockaddr->sa_family: %08x\n", sa->sa_family);
dbgprint("For reference, AF_INET6 is: %08x\n", AF_INET6);

	return sa;
}

#include <sys/socket.h>
#include <netdb.h>

char* resolve_service(inet_prefix* addr, int port){
	unsigned int buflen = 128;
	char* buf = (char*) calloc(buflen, sizeof(char));

dbgprint("[DEBUG] Resolving a service...\n");

	if(port == 0){
		free(buf);
		return NULL;
	}

	unsigned int addrlen;
	struct sockaddr* sa = inet_prefix_to_sockaddr(addr, &addrlen, port);

	int r = getnameinfo(sa, addrlen, 0, 0, buf, buflen, 0);
	free(sa);
	if(r != 0){
dbgprint("[DEBUG] getnameinfo returned err: %s\n", gai_strerror(r));
		return NULL;
	}

	return buf;
}

//Not really hostname, moreso domain name.
char* resolve_hostname(const inet_prefix* addr, int port){
	unsigned int buflen = 128;
	char* buf = (char*) calloc(buflen, sizeof(char));

dbgprint("[DEBUG] Resolving a hostname...\n");

	if(addr->family == AF_INET){
		if(addr->data[0] == '\0'){
dbgprint("[DEBUG] addr is zero! Returning NULL...\n");
			free(buf);
			return NULL;
		}
	}

	unsigned int addrlen;
	struct sockaddr* sa = inet_prefix_to_sockaddr(addr, &addrlen, port);

	int r = getnameinfo(sa, addrlen, buf, buflen, 0, 0, 0);
	free(sa);
	if(r != 0){
dbgprint("[DEBUG] getnameinfo returned err: %s\n", gai_strerror(r));
		return NULL;
	}
dbgprint("[DEBUG] getnameinfo returned something! It is: %s\n", buf);

	return buf;
}

static const char* tmr_name[] = {
	"off",
	"on",
	"keepalive",
	"timewait",
	"persist",
	"unknown"
};

static const char* print_ms_timer(int timeout){
	static char buf[64];
	int secs, msecs, minutes;

	if(timeout < 0){
		timeout = 0;
	}

	secs = timeout / 1000;
	minutes = secs / 60;
	secs = secs % 60;
	msecs = timeout % 1000;

	buf[0] = '\0';

	if(minutes){
		msecs = 0;
		snprintf(buf, sizeof(buf)-16, "%dmin", minutes);
		if(minutes > 9){
			secs = 0;
		}
	}
	if(secs){
		if(secs > 9){
			msecs = 0;
		}
		sprintf(buf+strlen(buf), "%d%s", secs, msecs ? "." : "sec");
	}
	if(msecs){
		sprintf(buf+strlen(buf), "%03dms", msecs);
	}

	return buf;
}

//For _SC_CLK_TCK.
#include <unistd.h>

int get_user_hz(){
	return sysconf(_SC_CLK_TCK);
}

static const char* print_hz_timer(int timeout){
	int hz = get_user_hz();
	return print_ms_timer(((timeout*1000) + hz-1)/hz);
}

struct user_ent{
	struct user_ent* next;
	unsigned int ino;
	int pid;
	int fd;
	char process[0];
};

#define USER_ENT_HASH_SIZE 256
struct user_ent* user_ent_hash[USER_ENT_HASH_SIZE];

static int user_ent_hashfn(unsigned int ino){
	int val = (ino >> 24) ^ (ino >> 16) ^ (ino >> 8) ^ ino;

	return val & (USER_ENT_HASH_SIZE - 1);
}

static void user_ent_add(unsigned int ino, const char* process, int pid, int fd){
	struct user_ent* p;
	struct user_ent** pp;
	int str_len;

	str_len = strlen(process) + 1;
	p = (struct user_ent*) malloc(sizeof(struct user_ent) + str_len);
	if(!p){
		abort();
	}

	p->next = NULL;
	p->ino = ino;
	p->pid = pid;
	p->fd = fd;

	strcpy(p->process, process);
	pp = &user_ent_hash[user_ent_hashfn(ino)];
	p->next = *pp;
	*pp = p;
}

//For DIR.
#include <dirent.h>

//q: What even is all of this?
//a: I don't know exactly, but it involves hashing and regex, so I assume it's pretty complicated.
//(It sifts through file descriptors a particular proc has open...)
static void user_ent_hash_build(){
	const char* root = getenv("PROC_ROOT") ? : "/proc";
	struct dirent* d;
	char name[1024];
	int nameoff;
	DIR* dir;

	strcpy(name, root);
	if(strlen(name) == 0 || name[strlen(name)-1] != '/'){
		strcat(name, "/");
	}

	nameoff = strlen(name);

	dir = opendir(name);
	if(!dir){
		return;
	}

	while((d = readdir(dir)) != NULL){
		struct dirent* d1;
		char process[16];
		int pid, pos;
		DIR* dir1;
		char c;		//Rank censorship I tell you!

		if(sscanf(d->d_name, "%d%c", &pid, &c) != 1){
			continue;
		}

		sprintf(name + nameoff, "%d/fd/", pid);

		pos = strlen(name);

		if((dir1 = opendir(name)) == NULL){
			continue;
		}

		process[0] = '\0';

		while((d1 = readdir(dir1)) != NULL){
			const char* pattern = "socket:[";
			unsigned int ino;
			char lnk[64];
			int fd;
			ssize_t link_len;

			if(sscanf(d1->d_name, "%d%c", &fd, &c) != 1){
				continue;
			}

			sprintf(name+pos, "%d", fd);

			link_len = readlink(name, lnk, sizeof(lnk)-1);
			if(link_len == 1){
				continue;
			}
			lnk[link_len] = '\0';

			if(strncmp(lnk, pattern, strlen(pattern))){
				continue;
			}

			sscanf(lnk, "socket:[%u]", &ino);

			if(process[0] == '\0'){
				char tmp[1024];
				FILE* fp;

				snprintf(tmp, sizeof(tmp), "%s/%d/stat", root, pid);
				if((fp = fopen(tmp, "r")) != NULL){
					fscanf(fp, "%*d (%[^)])", process);
					fclose(fp);
				}
			}

			user_ent_add(ino, process, pid, fd);
		}
		closedir(dir1);
	}
	closedir(dir);
}

static int find_users(unsigned ino, char* buf, int buflen){
	struct user_ent* p;
	int cnt = 0;
	char* ptr;

	if(!ino){
		return 0;
	}

	p = user_ent_hash[user_ent_hashfn(ino)];
	ptr = buf;
	while(p){
		if(p->ino != ino){
			goto next;
		}

		if(ptr - buf >= buflen - 1){
			break;
		}

		snprintf(ptr, buflen - (ptr - buf),
				"(\"%s\",%d,%d),", p->process, p->pid, p->fd);
		ptr += strlen(ptr);
		cnt++;

next:
		p = p->next;
	}

	if(ptr != buf){
		ptr[-1] = '\0';
	}

	return cnt;
}

char* port_to_string(int port){
	char* buf = (char*) calloc(64, sizeof(char));

	if(port == 0){
		buf[0] = '*';
		buf[1] = '\0';
		return buf;
	}

	sprintf(buf, "%u", port);
	return buf;
}

#include "options.h"

//Originally "tcp_show_line", but we print our info in window_demon instead.
int model_angel::tcp_parse_proc_line(char* line, int AF){
	struct tcpstat s;
	char* loc;
	char* rem;
	char* data;
	char opt[256];
	int n;
	char* p;

	if((p = strchr(line, ':')) == NULL){
		dbgprint("[MODEL_ANGEL] Malformed /proc/net/tcp line.\n");
		return -1;
	}
	loc = p + 2;

	if((p = strchr(loc, ':')) == NULL){
		dbgprint("[MODEL_ANGEL] Malformed /proc/net/tcp line.\n");
		return -1;
	}
	p[5] = '\0';
	rem = p + 6;

	if((p = strchr(rem, ':')) == NULL){
		dbgprint("[MODEL_ANGEL] Malformed /proc/net/tcp line.\n");
		return -1;
	}
	p[5] = '\0';
	data = p + 6;

	/*	filter stuff that we are ignoring.
	do{
		int state = (data[1] >= 'A') ? (data[1] - 'A' + 10) : (data[1] - '0');


	}while(0);
	*/

	s.local.family = s.remote.family = AF;
	//IPv4
	if(AF == AF_INET){
		sscanf(loc, "%x:%x", s.local.data, (unsigned*)&s.local_port);
		sscanf(rem, "%x:%x", s.remote.data, (unsigned*)&s.remote_port);
		s.local.bytelen = s.remote.bytelen = 4;
	}
	//IPv6
	else{
		sscanf(loc, "%08x%08x%08x%08x:%x",
				s.local.data,
				s.local.data+1,
				s.local.data+2,
				s.local.data+3,
				&s.local_port);
		sscanf(rem, "%08x%08x%08x%08x:%x",
				s.remote.data,
				s.remote.data+1,
				s.remote.data+2,
				s.remote.data+3,
				&s.remote_port);
		s.local.bytelen = s.remote.bytelen = 16;
	}

	//if(run_ssfilter...)
	
	opt[0] = '\0';
	n = sscanf(data, "%x %x:%x %x:%x %x %d %d %u %d %llx %d %d %d %d %d %[^\n]\n",
			&s.state, &s.wq, &s.rq,
			&s.timer, &s.timeout, &s.retrs, &s.uid, &s.probes, &s.ino,
			&s.refcnt, &s.sk, &s.rto, &s.ato, &s.qack,
			&s.cwnd, &s.ssthresh, opt);

	if(n < 17){
		opt[0] = '\0';
	}

	if(n < 12){
		s.rto = 0;
		s.cwnd = 2;
		s.ssthresh = -1;
		s.ato = s.qack = 0;
	}

	//Testing via print.
	//Originally the "20"s are more calculated variables.
	/*
	fprintf(stderr, "%-*s ", 20, "tcp");
	fprintf(stderr, "%-*s ", 20, sstate_name[s.state]);

	fprintf(stderr, "%-6d %-6d", s.rq, s.wq);

	addr_print(&s.local, s.local_port);
	addr_print(&s.remote, s.remote_port);

	if(1){		//show_options
		//TODO: why?
		if(s.timer > 4){
			s.timer = 5;
		}

		fprintf(stderr, " timer:(%s,%s,%d)", tmr_name[s.timer], print_hz_timer(s.timeout), s.timer != 1 ? s.probes : s.retrs);
	}

	if(1){		//show_tcpinfo
		int hz = get_user_hz();
		if(s.rto && s.rto != 3*hz){
			fprintf(stderr, " rto:%g", (double)s.rto/hz);
		}
		if(s.ato){
			fprintf(stderr, " ato:%g", (double)s.ato/hz);
		}
		if(s.cwnd != 2){
			fprintf(stderr, " cwnd:%d", s.ssthresh);
		}
		if(s.qack/2){
			fprintf(stderr, " qack:%d", s.qack/2);
		}
		if(s.qack&1){
			fprintf(stderr, " bidir");
		}
	}

	if(1){		//show_users
		char ubuf[4096];
		if(find_users(s.ino, ubuf, sizeof(ubuf)) > 0){
			fprintf(stderr, " users:(%s)", ubuf);
		}
	}

	if(1){		//show_details
		if(s.uid){
			fprintf(stderr, " uid:%u", (unsigned)s.uid);
		}
		fprintf(stderr, " ino:%u", s.ino);
		fprintf(stderr, " sk:%llx", s.sk);
		if(opt[0]){
			fprintf(stderr, " opt:\"%s\"", opt);
		}
	}

	fprintf(stderr, "\n");*/

	//Load info into conn_entry.
	conn_entry* entry = (conn_entry*) calloc(1, sizeof(conn_entry));

	entry->netid = "TCP";
	entry->state = sstate_name[s.state];

	//Local and remote version should be the same.
	entry->ip_ver = s.local.family;

	//Local
	if(always_resolve_hostnames){
		entry->local_hostname = resolve_hostname(&s.local, s.local_port);
	}

	unsigned int buflen = 128;
	char* buf = (char*) calloc(buflen, sizeof(char));
	inet_ntop(s.local.family, s.local.data, buf, buflen);
	entry->local_addr = buf;

	if(always_resolve_services){
		entry->local_service = resolve_service(&s.local, s.local_port);
	}
	entry->local_port = port_to_string(s.local_port);

	//Remote
	if(always_resolve_hostnames){
		entry->rem_hostname = resolve_hostname(&s.remote, s.remote_port);
	}

	buf = (char*) calloc(buflen, sizeof(char));
	inet_ntop(s.remote.family, s.remote.data, buf, buflen);
	entry->rem_addr = buf;

	if(always_resolve_services){
		entry->rem_service = resolve_service(&s.remote, s.remote_port);
	}
	entry->rem_port = port_to_string(s.remote_port);

	this->connections.push_back(entry);

	return 0;
}

int model_angel::fetch_tcp_data(){
	//We could use netlink, but instead we shall parse /proc/net/tcp.
	//Code stolen from ss.c
	//
	//"Sigh... We have to parse /proc/net/tcp..."
	dbgprint("Fetching tcp data from /proc/net/tcp...\n");

	FILE* fp = NULL;
	char* buf = NULL;
	int bufsize = 64*1024;

	int r = get_good_buffer(&buf, &bufsize);
	if(r == -1){
		dbgprint("Could not allocate memory to read in system values.\n");
		goto outerr;
	}

	//IPv4
	if((fp = net_tcp_open()) == NULL){
		goto outerr;
	}
	setbuffer(fp, buf, bufsize);
	if(open_proc_tcp(fp, AF_INET)){
		goto outerr;
	}
	fclose(fp);

	//IPv6
	if((fp = net_tcp6_open()) == NULL){
		goto outerr;
	}
	setbuffer(fp, buf, bufsize);
	if(open_proc_tcp(fp, AF_INET6)){
		goto outerr;
	}
	fclose(fp);

	free(buf);

	return 0;

outerr:
	do{
		int saved_errno = errno;
		dbgprint("Could not fetch_tcp_data from /proc.\n");
		if(buf){
			free(buf);
		}
		if(fp){
			fclose(fp);
		}
		errno = saved_errno;
		return -1;
	}while(0);
}

//Originally "dgram_show_line", but we print our info in window_demon instead.
int model_angel::udp_parse_proc_line(char* line, int AF){
	struct tcpstat s;
	char* loc;
	char* rem;
	char* data;
	char opt[256];
	int n;
	char *p;

	if((p = strchr(line, ':')) == NULL){
		dbgprint("Malformed /proc/net/udp line.\n");
		return -1;
	}
	loc = p+2;

	if((p = strchr(loc, ':')) == NULL){
		dbgprint("Malformed /proc/net/udp line.\n");
		return -1;
	}
	p[5] = '\0';
	rem = p+6;

	if((p = strchr(rem, ':')) == NULL){
		dbgprint("Malformed /proc/net/udp line.\n");
		return -1;
	}
	p[5] = '\0';
	data = p+6;

	/* Filtering code that, for now, we do not care about.
	do {
		int state = (data[1] >= 'A') ? (data[1] - 'A' + 10) : (data[1] - '0');

		if (!(f->states & (1<<state)))
			return 0;
	} while (0);
	*/

	s.local.family = s.remote.family = AF;
	//IPv4
	if(AF == AF_INET){
		sscanf(loc, "%x:%x", s.local.data, (unsigned*)&s.local_port);
		sscanf(rem, "%x:%x", s.remote.data, (unsigned*)&s.remote_port);
		s.local.bytelen = s.remote.bytelen = 4;
	}
	//IPv6
	else{
		sscanf(loc, "%08x%08x%08x%08x:%x",
		       s.local.data,
		       s.local.data+1,
		       s.local.data+2,
		       s.local.data+3,
		       &s.local_port);
		sscanf(rem, "%08x%08x%08x%08x:%x",
		       s.remote.data,
		       s.remote.data+1,
		       s.remote.data+2,
		       s.remote.data+3,
		       &s.remote_port);
		s.local.bytelen = s.remote.bytelen = 16;
	}

	/* more filtering code
	if (f->f && run_ssfilter(f->f, &s) == 0)
		return 0;
	*/

	opt[0] = '\0';
	n = sscanf(data, "%x %x:%x %*x:%*x %*x %d %*d %u %d %llx %[^\n]\n",
	       &s.state, &s.wq, &s.rq,
	       &s.uid, &s.ino,
	       &s.refcnt, &s.sk, opt);

	if(n < 9){
		opt[0] = '\0';
	}

	/*if(netid_width){
		printf("%-*s ", netid_width, dg_proto);
	}
	if(state_width){
		printf("%-*s ", state_width, sstate_name[s.state]);
	}

	printf("%-6d %-6d ", s.rq, s.wq);

	formatted_print(&s.local, s.lport);
	formatted_print(&s.remote, s.rport);

	if(show_users){
		char ubuf[4096];
		if(find_users(s.ino, ubuf, sizeof(ubuf)) > 0){
			printf(" users:(%s)", ubuf);
		}
	}

	if(show_details){
		if(s.uid){
			printf(" uid=%u", (unsigned)s.uid);
		}
		printf(" ino=%u", s.ino);
		printf(" sk=%llx", s.sk);
		if(opt[0]){
			printf(" opt:\"%s\"", opt);
		}
	}
	printf("\n");
	*/

	//Load info into conn_entry.
	conn_entry* entry = (conn_entry*) calloc(1, sizeof(conn_entry));

	entry->netid = "UDP";
	entry->state = sstate_name[s.state];

	//Local and remote version should be the same.
	entry->ip_ver = s.local.family;

	//Local
	if(always_resolve_hostnames){
		entry->local_hostname = resolve_hostname(&s.local, s.local_port);
	}

	unsigned int buflen = 128;
	char* buf = (char*) calloc(buflen, sizeof(char));
	inet_ntop(s.local.family, s.local.data, buf, buflen);
	entry->local_addr = buf;

	if(always_resolve_services){
		entry->local_service = resolve_service(&s.local, s.local_port);
	}
	entry->local_port = port_to_string(s.local_port);

	//Remote
	if(always_resolve_hostnames){
		entry->rem_hostname = resolve_hostname(&s.remote, s.remote_port);
	}

	buf = (char*) calloc(buflen, sizeof(char));
	inet_ntop(s.remote.family, s.remote.data, buf, buflen);
	entry->rem_addr = buf;

	if(always_resolve_services){
		entry->rem_service = resolve_service(&s.remote, s.remote_port);
	}
	entry->rem_port = port_to_string(s.remote_port);

	this->connections.push_back(entry);

	return 0;
}

int model_angel::fetch_udp_data(){
	dbgprint("Fetching udp data from /proc/net/udp...\n");

	FILE *fp = NULL;

	/*if(!getenv("PROC_NET_UDP") && !getenv("PROC_ROOT")
	    && inet_show_netlink(f, NULL, IPPROTO_UDP) == 0){
		return 0;
	}*/

	//dg_proto = UDP_PROTO;

	//if (f->families&(1<<AF_INET)) {
		if((fp = net_udp_open()) == NULL){
			goto outerr;
		}
		if(open_proc_udp(fp, AF_INET)){
			goto outerr;
		}
		fclose(fp);
	//}

	//if ((f->families&(1<<AF_INET6)) &&
	//    (fp = net_udp6_open()) != NULL) {
		if((fp = net_udp6_open()) == NULL){
			goto outerr;
		}
		if(open_proc_udp(fp, AF_INET6)){
			goto outerr;
		}
		fclose(fp);
	//}
	return 0;

outerr:
	do{
		int saved_errno = errno;
		if(fp){
			fclose(fp);
		}
		errno = saved_errno;
		return -1;
	}while(0);
}

int model_angel::fetch_connections(){
	//Deallocate and clear.
	for(conn_entry* entry : connections){
		free_conn_entry(entry);
	}
	connections.clear();

	int r;

	r = fetch_tcp_data();
	if(r == -1){
		dbgprint("[MODEL_ANGEL] Could not fetch_tcp_data() from system.\n");
		return -1;
	}

	r = fetch_udp_data();
	if(r == -1){
		dbgprint("[MODEL_ANGEL] Could not fetch_udp_data() from system.\n");
	}
	//fetch_raw_data();
	
	return 0;
}

#define MSG_BUFLEN 30

void model_angel::add_message_to_log(int type, char* text){
	message* p = (message*) calloc(1, sizeof(message));

	p->type = type;
	//This was already allocated on the heap by the networking agent.
	p->text = text;

	message_log.insert(message_log.begin(), p);
}

int model_angel::check_networking_agent(){
	int r = net_agent.check_for_incoming_connections();
	if(r == -1){
		return -1;
	}

	message messages[MSG_BUFLEN];

	int num_messages = net_agent.check_for_messages(messages, MSG_BUFLEN);
	if(num_messages == -1){
		return -1;
	}

	for(unsigned int i = 0; i < num_messages; i++){
		if(messages[i].text != NULL){
			dbgprint("[MODEL_ANGEL] Received a message! Text reads: %s\n", messages[i].text);
		}
		switch(messages[i].type){
			case INFO:
				//this->memo();
				this->add_message_to_log(INFO, messages[i].text);
				break;
			case WARN:
				this->warn();
				this->add_message_to_log(WARN, messages[i].text);
				break;
			case ALERT:
				this->alert(messages[i].text);
				this->add_message_to_log(ALERT, messages[i].text);
				break;
			default:
				dbgprint("[MODEL_ANGEL] Message with unrecognised type somehow got through the filters!\n");
				break;
		}
	}

	return 0;
}

#include "common.h"

int model_angel::update(){
	int r;
	if(ticker % 2 == 1){
		r = fetch_connections();
		if(r == -1){
			dbgprint("Could not fetch_connections() from system.\n");
			return -1;
		}
	}

	r = check_networking_agent();
	if(r == -1){
		dbgprint("Could not communicate with networking agent.\n");
		return -1;
	}

	return 0;
}
