#include <stdio.h>                      
#include "wiiuse.h"                     
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>                     

#define MAX_WIIMOTES				1
#define PIPE					"wiidata"
#define SAMPLES					"samples"


void handle_event(struct wiimote_t* wm, char * save_path, int * num_samples, int mode) {	
	static int prev_state = 0;
	static int current_state = 0; //State of 1 means gesture data is currently being collected
                                  //State of 0 means gesture data is not currently being collected
	int fd; //Holds file descriptor for pipe
	char message[50] = ""; //Holds data to send through pipe

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) { //Detects the b button being pressed
		current_state = 1;	
		if(!(wm->exp.type == EXP_NUNCHUK)){ //If motion_plus isn't set, set both it and motion sensing
			wiiuse_set_motion_plus(wm, 1);
			wiiuse_motion_sensing(wm, 1);
		}

		sprintf(message, "%f %f %f %d %d %d\n",
			wm->gforce.x, wm->gforce.y, wm->gforce.z,
			wm->exp.mp.raw_gyro.pitch, wm->exp.mp.raw_gyro.roll,
			wm->exp.mp.raw_gyro.yaw);

		//X Y Z Pitch Roll Yaw
		if(mode == 0){//print to pipe			
			fd = open(PIPE, O_WRONLY);
			write(fd, message, strlen(message) +1);
			close(fd);
		}else{//print to file
			FILE * fp;
			char filepath[100]="";
			sprintf(filepath,"%s%d",save_path, *num_samples); //Create path to file for current training sample
			fp = fopen(filepath, "ab+");
			fputs(message, fp);
			fclose(fp);
		
		}
		printf( "%d %d %d %d %d %d\n gforce: %f %f %f\n",
			wm->accel.x, wm->accel.y, wm->accel.z,
			wm->exp.mp.raw_gyro.pitch, wm->exp.mp.raw_gyro.roll,
			wm->exp.mp.raw_gyro.yaw,
            wm->gforce.x, wm->gforce.y, wm->gforce.z);
        				
	} else{
		current_state=0; //B is not currently being pressed
	}

	if(current_state != prev_state){ //If b went from being pressed to not or vice versa
		if(current_state){
			if(mode ==0){ //Indicate that data transmission has started
				fd = open(PIPE, O_WRONLY);
				write(fd, "start\n", strlen("start\n") +1);
				close(fd);
			}		
		}else{
			if(mode==0){ //Indicate that data transmission has stopped
				fd = open(PIPE, O_WRONLY);
				write(fd, "stop\n", strlen("stop\n") +1);
				close(fd);
			}else{
				*num_samples = *num_samples + 1; //If the b button was released,
                                                 //increment the num of training samples
			}
		}
	}
	prev_state = current_state;
	
}

int main(int argc, char** argv) {
	wiimote** wiimotes;
	int found, connected, mode, commandType, num_samples;
	//Mode = 1 save training data. Mode = 0 send data to pipe 
	char save_path[100] = "";
	
	
	if(argc == 1){ //Send data through pipe
		mode = 0;
	} else if(argc == 2){ //Save data as simple gesture data
        mode = 1;
        commandType = 0;
    }else if(argc == 3){ //Save data as either simple or advanced gesture data
		mode = 1;
		commandType = atoi(argv[2]); //sets whether to save simple or advanced gestures
	} else{
		printf("The program can be run in one of three ways:\n"
                "wii_capture\n"
                "wii_capture <gesture name>\n"
                "wii_capture <gesutre name> <mode(0/1)>\n");
		return -1;
	} 

	if(mode == 1){
		FILE * ls;
		char command[100] = "ls -l "; //Lists the number of files in a directory (plus one extra line)
		char size[10]; //number of files in directory

		strcat(save_path, "samples/");
		mkdir(save_path, 0755);
		if(commandType == 0){
			strcat(save_path, "simple/");
		}else if(commandType == 1){
			strcat(save_path, "advanced/");
		} else{
			printf("Invalid argument, must either be 0 or 1\n");
			return -1;
		}
		mkdir(save_path, 0755);
		strcat(save_path, argv[1]);
		strcat(save_path, "/");
		mkdir(save_path, 0755);

		strcat(command, save_path);
		strcat(command, " | wc -l"); //Counts the number of lines piped into it
		
		ls = popen(command, "r"); //Run ls -l | wc -l
		fread(&size, sizeof(char), 10, ls); //Get output from command
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
	usleep(200000);
	wiiuse_rumble(wiimotes[0], 0);
	
	wiiuse_set_flags(wiimotes[0], WIIUSE_CONTINUOUS,0 );
	while (WIIMOTE_IS_CONNECTED(wiimotes[0])) {
		if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
			switch (wiimotes[0]->event) {
				case WIIUSE_EVENT:
					/* a generic event occurred */
					handle_event(wiimotes[0], save_path, &num_samples, mode);
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
