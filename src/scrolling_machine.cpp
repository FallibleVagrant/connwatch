#include "scrolling_machine.h"

/*
Behold! The Scrolling Machine!
Braveness upon you if you are here to debug it.
Each view_page holds a variable number of entries,
which makes computing which page you are on non-trivial.

The first page, for example, may have MAX_ITEMS_PER_PAGE entries, or
MAX_ITEMS_PER_PAGE - 1 entries, because the little "vvv" takes up
the space of one entry.

Middle pages (not the first or last page.) always take up MAX_ITEMS_PER_PAGE - 2 entries.
This is because they have both the "^^^" and the "vvv", taking up two spaces.

The last page takes up between two and MAX_ITEMS_PER_PAGE - 1 entries.
A last page cannot have just one entry, as it would instead be used to fill the previous page.

The point is to derive everything from this->choice, to avoid state and also because I wanted the challenge of implementing this.
*/

void compute_range_of_page(
		const unsigned int MAX_ITEMS_PER_PAGE,
		const unsigned int num_entries,
		const int choice,
		int* shown_page,
		int* num_view_pages,
		int* max_items_on_shown_page,
		int* starting_entry_on_shown_page,
		int* shown_window_choice){


	*shown_page = 0;
	*num_view_pages = 0;
	*max_items_on_shown_page = 0;
	*shown_window_choice = 0;

	//One view_page exists.
	if(num_entries <= MAX_ITEMS_PER_PAGE){
		*num_view_pages = 1;

		*shown_window_choice = choice;
		*shown_page = 0;
		*max_items_on_shown_page = MAX_ITEMS_PER_PAGE;
	}
	else{
		//Two view_pages exist, but not more.
		//(num_entries > MAX_ITEMS_PER_PAGE)
		if(num_entries <= (2 * (MAX_ITEMS_PER_PAGE - 1))){
			*num_view_pages = 2;
			//Still on the first view_page.
			if(choice < MAX_ITEMS_PER_PAGE - 1){
				*shown_window_choice = choice;
				*shown_page = 0;
				*max_items_on_shown_page = MAX_ITEMS_PER_PAGE - 1;
			}
			else{
				//On the second (and last) view_page.
				//(choice > MAX_ITEMS_PER_PAGE - 1)
				*shown_page = 1;
				*shown_window_choice = choice - (MAX_ITEMS_PER_PAGE - 1);
				*max_items_on_shown_page = MAX_ITEMS_PER_PAGE - 1;
			}
		}
		//More than two view_pages exist.
		//(num_entries > MAX_ITEMS_PER_PAGE)
		//					&&
		//(num_entries > (2 * (MAX_ITEMS_PER_PAGE - 1)))
		else{
			int num_items_excl_first_page = num_entries - (MAX_ITEMS_PER_PAGE - 1);
			int num_middle_pages = num_items_excl_first_page / (MAX_ITEMS_PER_PAGE - 2);

			//If the last page is filled, or (filled - 1), it was accidentally counted in num_middle_pages.
			//NOTE: the last page can never have only one connection, as it would instead
			//be appended to the page before it.
			if(num_items_excl_first_page % (MAX_ITEMS_PER_PAGE - 2) == 1
					|| num_items_excl_first_page % (MAX_ITEMS_PER_PAGE - 2) == 0){
				num_middle_pages--;
			}

			*num_view_pages = num_middle_pages + 2;

			//Still on the first view_page.
			if(choice <= MAX_ITEMS_PER_PAGE - 1){
				*shown_window_choice = choice;
				*shown_page = 0;
				*max_items_on_shown_page = MAX_ITEMS_PER_PAGE - 1;
			}
			else{
				int num_items_before_last_page = (MAX_ITEMS_PER_PAGE - 1) + (num_middle_pages * (MAX_ITEMS_PER_PAGE - 2));

				//Past the first view_page, but not on the last.
				//(choice > MAX_ITEMS_PER_PAGE - 1)
				if(choice <= num_items_before_last_page){
					//We know we are not on the last page, so this calc will never count the last page.
					int num_middle_pages_passed = (choice - (MAX_ITEMS_PER_PAGE - 1)) / (MAX_ITEMS_PER_PAGE - 2);

					*shown_page = num_middle_pages_passed + 1;
					*shown_window_choice = (choice - (MAX_ITEMS_PER_PAGE - 1)) % (MAX_ITEMS_PER_PAGE - 2);
					*max_items_on_shown_page = MAX_ITEMS_PER_PAGE - 2;
				}
				//On the last view_page.
				//(choice > MAX_ITEMS_PER_PAGE - 1)
				//				&&
				//(choice > num_items_before_last_page)
				else{
					*shown_window_choice = choice - num_items_before_last_page;
					*shown_page = 1 + num_middle_pages;
					*max_items_on_shown_page = MAX_ITEMS_PER_PAGE - 1;
				}
			}
		}
	}

	//Determine the first item to show on the shown_page.
	if(shown_page == 0){
		*starting_entry_on_shown_page = 0;
	}
	if(*shown_page >= 1){
		int num_middle_pages_passed = *shown_page - 1;
		*starting_entry_on_shown_page = (MAX_ITEMS_PER_PAGE - 1) + (num_middle_pages_passed * (MAX_ITEMS_PER_PAGE - 2));
	}
}
