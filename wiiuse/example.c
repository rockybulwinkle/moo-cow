

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
#include <string.h>

#ifndef WIIUSE_WIN32
#include <unistd.h>                     /* for usleep */
#endif

#define MAX_WIIMOTES				1
#define PIPE					"wiidata"
#define SAMPLES					"samples"


/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 */
void handle_event(struct wiimote_t* wm, char * save_path, int * num_samples, int mode) {	
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
		if(mode == 0){//print to pipe			
			sprintf(message, "%d %d %d %d %d %d\n",
				wm->accel.x, wm->accel.y, wm->accel.z,
				wm->exp.mp.raw_gyro.pitch, wm->exp.mp.raw_gyro.roll,
				wm->exp.mp.raw_gyro.yaw);
	
			fd = open(PIPE, O_WRONLY);
			write(fd, message, strlen(message) +1);
			close(fd);
			printf("%s\n", message);
		}else{//print to file
			FILE * fp;
			char filepath[100]="";
			sprintf(filepath,"%s%d",save_path, *num_samples);
			fp = fopen(filepath, "ab+");
			fprintf(fp, "%d %d %d %d %d %d\n",
				wm->accel.x, wm->accel.y, wm->accel.z,
				wm->exp.mp.raw_gyro.pitch, wm->exp.mp.raw_gyro.roll,
				wm->exp.mp.raw_gyro.yaw);
			fclose(fp);
			printf( "%d %d %d %d %d %d\n",
				wm->accel.x, wm->accel.y, wm->accel.z,
				wm->exp.mp.raw_gyro.pitch, wm->exp.mp.raw_gyro.roll,
				wm->exp.mp.raw_gyro.yaw);		
		}

				
	} else{
		current_state=0;
	}

	if(current_state != prev_state){
		if(current_state){
			if(mode ==0){
				fd = open(PIPE, O_WRONLY);
				write(fd, "start\n", strlen("start\n") +1);
				close(fd);
			}		
		}else{
			if(mode==0){
				fd = open(PIPE, O_WRONLY);
				write(fd, "stop\n", strlen("stop\n") +1);
				close(fd);
			}else{
				*num_samples = *num_samples + 1;
			}
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
	int found, connected, mode, commandType, num_samples;
	//Mode = 1 save training data. Mode = 0 send data to pipe 
	char save_path[100] = "";
	
	
	if(argc == 1){
		mode = 0;
	} else if(argc == 3){
		mode = 1;
		commandType = atoi(argv[2]);
	} else{
		printf("This program only accepts zero or two  argument\n");
		return -1;
	} 

	if(mode == 1){
		FILE * ls;
		char command[100] = "ls -il ";
		char size[10];

		strcat(save_path, "samples/");
		mkdir(save_path, 0755);
		if(commandType == 1){
			strcat(save_path, "simple/");
		}else if(commandType == 2){
			strcat(save_path, "advanced/");
		} else{
			printf("Invalid argument\n");
			return -1;
		}
		mkdir(save_path, 0755);
		strcat(save_path, argv[1]);
		strcat(save_path, "/");
		mkdir(save_path, 0755);

		strcat(command, save_path);
		strcat(command, " | wc -l");
		
		ls = popen(command, "r");
		fread(&size, sizeof(char), 5, ls);
		num_samples = atoi(size) -1;
		pclose(ls);
	}	

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
					handle_event(wiimotes[0], save_path, &num_samples, mode);
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
