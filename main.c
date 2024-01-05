#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <xcb/xcb.h>
#include <xcb/randr.h>


char* get_monitor_name(xcb_connection_t* connection, const xcb_randr_monitor_info_t* monitor_info)
{
	/* 'name' must be freed ! */
	char* name = NULL;
	xcb_get_atom_name_reply_t* name_reply = xcb_get_atom_name_reply(connection,
														xcb_get_atom_name(connection, monitor_info->name),
														NULL);

	if(name_reply)
	{
		int name_len = xcb_get_atom_name_name_length(name_reply);
		if(name_len > 0)
		{
			name = malloc(name_len * sizeof(char));
			memcpy(name, xcb_get_atom_name_name(name_reply), name_len);
			name[name_len] = '\0';
		}

		free(name_reply);
	}

	return name;
}


void print_monitors_info(xcb_connection_t* connection, const xcb_randr_monitor_info_t* monitor_info)
{
	char* monitor_name = get_monitor_name(connection, monitor_info);
	
	if(monitor_info->primary)
		printf("*primary* ");

	printf("%s\n"
		   "    resolution\n"
		   "      %"PRIu16"x%"PRIu16"\n"
		   "    physical size\n"
		   "      %"PRIu32"x%"PRIu32"mm\n"
		   "    logical position\n"
		   "      x: %"PRIu16"\n"
		   "      y: %"PRIu16"\n",
			monitor_name,
			monitor_info->width,
			monitor_info->height,
			monitor_info->width_in_millimeters,
			monitor_info->height_in_millimeters,
			monitor_info->x,
			monitor_info->y);


	free(monitor_name);
}


int main(void)
{
	xcb_connection_t* connection = xcb_connect(NULL, NULL);
	const xcb_setup_t* setup = xcb_get_setup(connection);
	xcb_window_t root = xcb_setup_roots_iterator(setup).data->root;

	xcb_randr_get_monitors_reply_t* monitors_reply = xcb_randr_get_monitors_reply(connection,
																	xcb_randr_get_monitors(connection,
																						root,
																						0),
																	NULL);

	xcb_randr_monitor_info_iterator_t monitors_iterator = xcb_randr_get_monitors_monitors_iterator(monitors_reply);

	while(monitors_iterator.rem)
	{	
		xcb_randr_monitor_info_t* monitor_info = monitors_iterator.data;
		print_monitors_info(connection, monitor_info);
		xcb_randr_monitor_info_next(&monitors_iterator);
	}

	free(monitors_reply);
	xcb_disconnect(connection);
	return 0;
}
