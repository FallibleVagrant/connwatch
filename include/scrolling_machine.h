#ifndef SCROLLING_MACHINE_H
#define SCROLLING_MACHINE_H

void compute_range_of_page(
		const unsigned int MAX_ITEMS_PER_PAGE,
		const unsigned int num_entries,
		const unsigned int choice,
		int* shown_page,
		int* num_view_pages,
		unsigned int* max_items_on_shown_page,
		unsigned int* starting_entry_on_shown_page,
		int* shown_window_choice);


#endif
