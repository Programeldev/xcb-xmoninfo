#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <xcb/randr.h>



void FREE(void* pointer)
{
	free(pointer);
	pointer = NULL;
}


int main(void)
{
	xcb_connection_t* connection = xcb_connect(NULL, NULL);

	const xcb_setup_t* setup = xcb_get_setup(connection);
	const xcb_screen_t* screen = xcb_setup_roots_iterator(setup).data;

	xcb_window_t window = xcb_generate_id(connection);
	xcb_create_window(connection,
					0,
					window,
					screen->root,
					0,
					0,
					1,
					1,
					0,
					0,
					0,
					0,
					0 );
	
	xcb_flush(connection);

	xcb_randr_get_screen_resources_current_reply_t* screen_reply = xcb_randr_get_screen_resources_current_reply(connection,
																			xcb_randr_get_screen_resources_current(connection,
																												window),
																			NULL );

	if(screen_reply == NULL)
	{
		printf("Error: Couldn't get information about outputs from RandR.\n");
		return -1;
	}

	xcb_timestamp_t timestamp = screen_reply->config_timestamp;
	int len = xcb_randr_get_screen_resources_current_outputs_length(screen_reply);
	xcb_randr_output_t* randr_outputs = xcb_randr_get_screen_resources_current_outputs(screen_reply);

	for(int i = 0; i < len; i++)
	{
		xcb_randr_get_output_info_reply_t* output_reply = xcb_randr_get_output_info_reply(connection,
																		xcb_randr_get_output_info(connection, 
																					randr_outputs[i], 
																					timestamp),
																		NULL );
		if(output_reply == NULL)
		{
			fprintf(stderr, "Error: Failed to get information from output number %i.\n", i);
			continue;
		}

		if(output_reply->crtc == XCB_NONE || output_reply->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
		{
			fprintf(stderr, "Output number %i is disconnected.\n", i);
			continue;	
		}

		xcb_randr_get_crtc_info_reply_t* crtc_info_reply = xcb_randr_get_crtc_info_reply(connection,
																xcb_randr_get_crtc_info(connection,
																			output_reply->crtc,
																			timestamp),
																NULL );

		printf("Output #%i\n  axis position:\n    x = %i | y = %i \n  resolution:\n    %ix%i\n\n", i,
																			crtc_info_reply->x, 
																			crtc_info_reply->y, 
																			crtc_info_reply->width, 
																			crtc_info_reply->height );
	
		FREE(crtc_info_reply);
		FREE(output_reply);
	}

	FREE(screen_reply);

	xcb_disconnect(connection);

	return 0;
}
