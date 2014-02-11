

/**
 *	@file
 *
 *	@brief Example using the wiiuse API.
 *
 *	This file is an example of how to use the wiiuse library.
 */

#include <stdio.h>                      /* for printf */

#include "wiiuse.h"                     /* for wiimote_t, classic_ctrl_t, etc */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifndef WIIUSE_WIN32
#include <unistd.h>                     /* for usleep */
#endif

#define MAX_WIIMOTES				1
#define PIPE					"wiidata"


/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t* wm) {	
	static int prev_state = 0;
	static int current_state = 0;
	int fd;
	char message[50] = "";

	/* if a button is pressed, report it */
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) {
		current_state = 1;
		if(!(wm->exp.type == EXP_NUNCHUK)){
			wiiuse_set_motion_plus(wm, 1);
			wiiuse_motion_sensing(wm, 1);
		}
		//X Y Z Pitch Roll Yaw			
		sprintf(message, "%d %d %d %d %d %d\n",
			wm->accel.x, wm->accel.y, wm->accel.z,
			wm->exp.mp.raw_gyro.pitch, wm->exp.mp.raw_gyro.roll,
			wm->exp.mp.raw_gyro.yaw);

		fd = open(PIPE, O_WRONLY);
		write(fd, message, strlen(message) +1);
		close(fd);

		printf("%s\n", message);		
	} else{
		current_state=0;
	}

	if(current_state != prev_state){
		if(current_state){
			fd = open(PIPE, O_WRONLY);
			write(fd, "start\n", strlen("start\n") +1);
			close(fd);		
		}else{
			fd = open(PIPE, O_WRONLY);
			write(fd, "stop\n", strlen("stop\n") +1);
			close(fd);
		}
	}
	prev_state = current_state;
	
}


/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote* wm) {
	printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}

short any_wiimote_connected(wiimote** wm, int wiimotes) {
	int i;
	if (!wm) {
		return 0;
	}

	for (i = 0; i < wiimotes; i++) {
		if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
			return 1;
		}
	}

	return 0;
}


/**
 *	@brief main()
 *
 *	Connect to up to two wiimotes and print any events
 *	that occur on either device.
 */
int main(int argc, char** argv) {
	wiimote** wiimotes;
	int found, connected;

	wiimotes =  wiiuse_init(1);

	found = wiiuse_find(wiimotes, 1, 5);
	if (!found) {
		printf("No wiimotes found.\n");
		return 0;
	}

	connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
	if (connected) {
		printf("Connected to %i wiimotes (of %i found).\n", connected, found);
	} else {
		printf("Failed to connect to any wiimote.\n");
		return 0;
	}

	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
	wiiuse_rumble(wiimotes[0], 1);	

	
#ifndef WIIUSE_WIN32
	usleep(200000);
#else
	Sleep(200);
#endif

	wiiuse_rumble(wiimotes[0], 0);
	
	wiiuse_set_flags(wiimotes[0], WIIUSE_CONTINUOUS,0 );
	while (any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {
		if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {

			switch (wiimotes[0]->event) {
				case WIIUSE_EVENT:
					/* a generic event occurred */
					handle_event(wiimotes[0]);
					break;
				case WIIUSE_DISCONNECT:
				case WIIUSE_UNEXPECTED_DISCONNECT:
					/* the wiimote disconnected */
					handle_disconnect(wiimotes[0]);
					break;
				default:
					break;
			}
		}
	}

	wiiuse_cleanup(wiimotes, MAX_WIIMOTES
);

	return 0;
}
