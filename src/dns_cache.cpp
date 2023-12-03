#include "dns_cache.h"

#include <string.h>
#include <stdlib.h>

bool addrkey::operator<(const addrkey& rhs) const {
	return memcmp(addr, rhs.addr, 4 * 8) < 0;
}

dns_cache::dns_cache(){}

dns_cache::~dns_cache(){
	//TODO: free the cache, I guess...
}

void dns_cache::insert(const unsigned int* addr, char* hostname){
	addrkey key;

	memcpy(key.addr, addr, 4 * 8);
	
	if(cache.contains(key)){
		struct dnsvalue value;
		value = cache[key];

		//TODO: check for expiry.
		if(value.time_recorded){
			return;
		}
	}

	//Do the insert proper.
	struct dnsvalue* value = (struct dnsvalue*) calloc(1, sizeof(struct dnsvalue));
	char* copy = (char*) calloc(1, strlen(hostname) + 1);
	strcpy(copy, hostname);
	value->hostname = copy;
	//TODO: actually record the time.
	value->time_recorded = 1;
	cache[key] = *value;
}

char* dns_cache::search(const unsigned int* addr){
	addrkey key;

	memcpy(key.addr, addr, 4 * 8);

	if(cache.contains(key)){
		struct dnsvalue value;
		value = cache[key];

		//TODO: check for expiry.
		if(value.time_recorded){
			char* hostname = cache[key].hostname;
			char* r = (char*) calloc(1, strlen(hostname) + 1);
			strcpy(r, hostname);
			return r;
		}
	}

	return NULL;
}
