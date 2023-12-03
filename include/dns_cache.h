#ifndef DNS_CACHE_H
#define DNS_CACHE_H

#include <map>
#include <time.h>

class addrkey{
	public:
		unsigned int addr[8];
		bool operator<(const addrkey& rhs) const;
};

struct dnsvalue{
	char* hostname;
	time_t time_recorded;
};

class dns_cache{
	public:
		dns_cache();
		~dns_cache();

		void insert(const unsigned int* addr, char* hostname);
		char* search(const unsigned int* addr);
	private:
		std::map<addrkey, struct dnsvalue> cache;
};

#endif
