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

#include <time.h>
#include "common.h"

void dns_cache::insert(const unsigned int* addr, char* hostname){
	addrkey key;

	memcpy(key.addr, addr, 4 * 8);
	
	//Check if the value to insert is already cached.
	if(cache.contains(key)){
		struct dnsvalue value;
		value = cache[key];

		if(time(0) < value.expiry){
			//The same value is already cached, and isn't expired.
			return;
		}
		else{
			//It's cached, but expired. Free the value and insert with the updated hostname and new expiry.
			free(&value);
		}
	}

	//Do the insert proper.
	struct dnsvalue* value = (struct dnsvalue*) calloc(1, sizeof(struct dnsvalue));
	char* copy = (char*) calloc(1, strlen(hostname) + 1);
	strcpy(copy, hostname);
	value->hostname = copy;
	value->expiry = time(0) + (ticker % 600) + 3600;
	cache[key] = *value;
}

char* dns_cache::search(const unsigned int* addr){
	addrkey key;

	memcpy(key.addr, addr, 4 * 8);

	if(cache.contains(key)){
		struct dnsvalue value;
		value = cache[key];

		//Make sure it isn't expired.
		if(time(0) < value.expiry){
			char* hostname = cache[key].hostname;
			char* r = (char*) calloc(1, strlen(hostname) + 1);
			strcpy(r, hostname);
			return r;
		}
	}

	return NULL;
}
