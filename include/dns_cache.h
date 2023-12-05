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
	time_t expiry;	//set an expiry instead of initial time, so we never have to check if the system clock was set backwards for some reason. Extremely defensive and pedantic, but whatever.
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
