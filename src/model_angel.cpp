#include "model_angel.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>

#include <netinet/tcp.h>
#include <arpa/inet.h>

//#include "connection_entry.h"
#include "debug.h"

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

model_angel::model_angel(){
	//Ignore the error!
	get_slabstat(&slabstat);
}

model_angel::~model_angel(){}

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

int generic_record_read(FILE* fp, int (*worker)(char*, int), int AF){
	char line[256];

	dbgprint("Reading a /proc file from generic_record_read()...\n");

	//Skip header in /proc/whatever.
	if(fgets(line, sizeof(line), fp) == NULL){
		dbgprint("Could not read header of a /proc file.\n");
		goto outerr;
	}

	while(fgets(line, sizeof(line), fp) != NULL){
		int n = strlen(line);
		if(n == 0 || line[n-1] != '\n'){
			dbgprint("Failed to parse a line in a /proc file.\n");
			errno = EINVAL;
			return -1;
		}
		line[n-1] = '\0';

		if(worker(line, AF) < 0){
			return 0;
		}
	}

outerr:
	return ferror(fp) ? -1 : 0;
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
	int			probe;
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

//TODO: mk this name clearer
const char* format_host(int AF, int len, const void* addr, char* buf, int buflen){
	//dbgprint("Formatting address from binary to text...\n");
	//TODO: add this functionality.
	if(0){	//An option would be present here; for now we do nothing to resolve the hostname.
		;
	}

	//Originally returns rt_addr_n2a, which switches b/t different AFs, but here we do this.
	return inet_ntop(AF, addr, buf, buflen);
}

static const char* resolve_service(int port){
	static char buf[256];
	//static struct scache cache[256];

	if(port == 0){
		buf[0] = '*';
		buf[1] = '\0';
		return buf;
	}

	//TODO: actually add this functionality.
	if(0){	//An option would be present here; for now we do nothing to actually resolve a service.
		;
	}

	sprintf(buf, "%u", port);
	return buf;
}

//Originally "formatted_print"
void addr_print(const inet_prefix* a, int port){
	char buf[1024];
	const char* ap = buf;
	int est_len;

	int addr_width = 16;	//Originally addr_width is a more calculated value, but for now we set it to 20. TODO: figure out what addr_width actually is.
	est_len = addr_width;

	//IPv4
	if(a->family == AF_INET){
		if(a->data[0] == '\0'){
			buf[0] = '*';
			buf[1] = '\0';
		}
		else{
			ap = format_host(AF_INET, 4, a->data, buf, sizeof(buf));
		}
	}
	//IPv6
	else{
		ap = format_host(a->family, 16, a->data, buf, sizeof(buf));
		est_len = strlen(ap);
		if(est_len <= addr_width){
			est_len = addr_width;
		}
		else{
			//TODO: why?
			est_len = addr_width + ((est_len - addr_width + 3) / 4) * 4;
		}
	}

	fprintf(stderr, "%*s:%-*s ", est_len, ap, 20, resolve_service(port));	//orig. "20" would be "serv_width"
}

//Originally "tcp_show_line", but we print our info in window_demon instead, maybe?
int tcp_parse_proc_line(char* line, int AF){
	struct tcpstat s;
	char* loc;
	char* rem;
	char* data;
	char opt[256];
	int n;
	char* p;

	if((p = strchr(line, ':')) == NULL){
		dbgprint("Malformed /proc/net/tcp line.\n");
		return -1;
	}
	loc = p + 2;

	if((p = strchr(loc, ':')) == NULL){
		dbgprint("Malformed /proc/net/tcp line.\n");
		return -1;
	}
	p[5] = '\0';
	rem = p + 6;

	if((p = strchr(rem, ':')) == NULL){
		dbgprint("Malformed /proc/net/tcp line.\n");
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
			&s.timer, &s.timeout, &s.retrs, &s.uid, &s.probe, &s.ino,
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
	fprintf(stderr, "%-*s ", 20, "tcp");
	fprintf(stderr, "%-*s ", 20, sstate_name[s.state]);

	fprintf(stderr, "%-6d %-6d", s.rq, s.wq);

	addr_print(&s.local, s.local_port);
	addr_print(&s.remote, s.remote_port);

	if(1){		//show_options
	}

	fprintf(stderr, "\n");

	return 0;
}

int model_angel::fetch_tcp_data(){
	//We could use netlink, but instead we shall parse /proc/net/tcp.
	//Code stolen from ss.c
	//
	//"Sigh... We have to parse /proc/net/tcp..."

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
	if(generic_record_read(fp, tcp_parse_proc_line, AF_INET)){
		goto outerr;
	}
	fclose(fp);

	//IPv6
	if((fp = net_tcp6_open()) == NULL){
		goto outerr;
	}
	setbuffer(fp, buf, bufsize);
	if(generic_record_read(fp, tcp_parse_proc_line, AF_INET6)){
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

int model_angel::fetch_connections(){
	int r;

	r = fetch_tcp_data();
	if(r == -1){
		dbgprint("Could not fetch_tcp_data() from system.\n");
		return -1;
	}
	//fetch_udp_data();
	//fetch_raw_data();

	return 0;
}

int model_angel::update(){
	int r = fetch_connections();
	if(r == -1){
		dbgprint("Could not fetch_connections() from system.\n");
		return -1;
	}

	return 0;
}
