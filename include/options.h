#ifndef OPTIONS_H
#define OPTIONS_H

extern bool always_show_hostnames;
extern bool always_show_services;
extern bool always_resolve_hostnames;
extern bool always_resolve_services;

extern bool sound_enabled;

enum sort_method_types{
	SORT_NONE,
	SORT_NETID_ASC,
	SORT_NETID_DESC,
	SORT_STATE_ASC,
	SORT_STATE_DESC,
};

extern int sort_method;
extern int num_sort_methods;

#endif
