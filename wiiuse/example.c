

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

	/* if a button is pressed, report it */
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) {
		int fd;
		char message[50] = "";
		if(!(wm->exp.type == EXP_NUNCHUK)){
			wiiuse_set_motion_plus(wm, 1);
			wiiuse_motion_sensing(wm, 1);
		}
		//X Y Z Pitch Roll Yaw			
		sprintf(message, "%d %d %d %03.2f %03.2f %03.2f\n",
			wm->accel.x, wm->accel.y, wm->accel.z,
			wm->exp.mp.angle_rate_gyro.pitch, wm->exp.mp.angle_rate_gyro.roll,
			wm->exp.mp.angle_rate_gyro.yaw);

		fd = open(PIPE, O_WRONLY);
		write(fd, message, strlen(message) +1);
		close(fd);

		printf("%s\n", message);		

	}
	
	
}

void handle_ctrl_status(struct wiimote_t* wm) {
	printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	printf("attachment:      %i\n", wm->exp.type);
	printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
	printf("ir:              %i\n", WIIUSE_USING_IR(wm));
	printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
	printf("battery:         %f %%\n", wm->battery_level);
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

	/*
	 *	Initialize an array of wiimote objects.
	 *
	 *	The parameter is the number of wiimotes I want to create.
	 */
	wiimotes =  wiiuse_init(1);
	
	/*
	 *	Find wiimote devices
	 *
	 *	Now we need to find some wiimotes.
	 *	Give the function the wiimote array we created, and tell it there
	 *	are MAX_WIIMOTES wiimotes we are interested in.
	 *
	 *	Set the timeout to be 5 seconds.
	 *
	 *	This will return the number of actual wiimotes that are in discovery mode.
	 */
	found = wiiuse_find(wiimotes, 1, 5);
	if (!found) {
		printf("No wiimotes found.\n");
		return 0;
	}

	/*
	 *	Connect to the wiimotes
	 *
	 *	Now that we found some wiimotes, connect to them.
	 *	Give the function the wiimote array and the number
	 *	of wiimote devices we found.
	 *
	 *	This will return the number of established connections to the found wiimotes.
	 */
	connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
	if (connected) {
		printf("Connected to %i wiimotes (of %i found).\n", connected, found);
	} else {
		printf("Failed to connect to any wiimote.\n");
		return 0;
	}

	/*
	 *	Now set the LEDs and rumble for a second so it's easy
	 *	to tell which wiimotes are connected (just like the wii does).
	 */
	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
	wiiuse_rumble(wiimotes[0], 1);	

	
#ifndef WIIUSE_WIN32
	usleep(200000);
#else
	Sleep(200);
#endif

	wiiuse_rumble(wiimotes[0], 0);

	/*
	 *	This is the main loop
	 *
	 *	wiiuse_poll() needs to be called with the wiimote array
	 *	and the number of wiimote structures in that array
	 *	(it doesn't matter if some of those wiimotes are not used
	 *	or are not connected).
	 *
	 *	This function will set the event flag for each wiimote
	 *	when the wiimote has things to report.
	 */
	while (any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {
		if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
			/*
			 *	This happens if something happened on any wiimote.
			 *	So go through each one and check if anything happened.
			 */
			switch (wiimotes[0]->event) {
				case WIIUSE_EVENT:
					/* a generic event occurred */
					handle_event(wiimotes[0]);
					break;
				case WIIUSE_STATUS:
					handle_ctrl_status(wiimotes[0]);
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

	/*
	 *	Disconnect the wiimotes
	 */
	wiiuse_cleanup(wiimotes, MAX_WIIMOTES
);

	return 0;
}
