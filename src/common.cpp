#include "common.h"

unsigned int ticker = 761283810;

//Crash if OOM.
void* kalloc(size_t nmemb, size_t size){
	void* p = calloc(nmemb, size);
	if(p == NULL){
		errprint("Out of memory! Crashing...\n");
		exit(1);
	}

	return p;
}
