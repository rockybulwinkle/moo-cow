#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <float.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "fann.h"
#include "resample.h"

#define PATH_TO_WIIDATA "wiidata"
#define PATH_TO_WII_PROCESS "./cow_capture"

char  gestures[6][30] = {"down","left","leftrightleft", "right", "rightleftright", "up"}; 
int keepgoing=1;

void signal_handler(int sig){
	keepgoing=0;
}

pid_t launch_wii_process(){
	pid_t pid = fork();
	
	if(pid>=0){
		if(pid ==0){
			if(execlp(PATH_TO_WII_PROCESS, "wiimote", NULL) == -1){
				int err = errno;
				printf("Failed to execute wiimote process. ERRNO: %d\n",err);
				return -1;	
			}
		}
	} else{
		printf("Wiimote process failed to start\n");
		return -2;	
	}
	return pid;
}

int wait_on_signal(int pipe){
	char data[30];
	read(pipe, data, 30);
	if(!strcmp(data,"ready")){
		return 1;
	}
	return 0;
}

int process(struct fann *ann1, struct fann *ann2, int fd){
	char data[100];
	int i, j, k;
	char * token;
	int maxsize = 10;
	int currentsize=0;
	int gesture_complete =0;
	int num_input_ann1 = ann1->num_input;
	int num_output_ann1 = ann1->num_output;
	float high;
	int high_index;
	float input_ann1[num_input_ann1];
	float ** output_ann1;
	float * input_ann2;
	float * output_ann2;
	float * temp_output;
	//allocate memory for outputs
	output_ann1 = (float **) malloc(maxsize*sizeof(float *));

	while(!gesture_complete){

		if(read(fd,data,100)!=0){ //read from pipe
			i=0;
//			printf("%s\n",data);
			token = strtok(data, " ");
			while(token != NULL){ //save to input[]
				if(!strcmp(token,"stop")){
					gesture_complete = 1; //check for stop signal if gesture is complete
					break;
				}else if(!strcmp(token, "quit")){
					return 0;
				}else{
					input_ann1[i++] = atof(token);
					token = strtok(NULL, " ");
				}
			}
			if(!gesture_complete){
				//Run input through first layer
				temp_output = fann_run(ann1, input_ann1);
				output_ann1[currentsize] = (float *) malloc(num_output_ann1*sizeof(float));
				high = FLT_MIN;
				high_index =0;
				for(k=0; k<num_output_ann1; k++){
					if(temp_output[k]>high){
						high = temp_output[k];
						high_index =k;
					}
//					output_ann1[currentsize][k] = temp_output[k];
				}

				for(k=0; k<num_output_ann1; k++){
					if(k == high_index){
						output_ann1[currentsize][k] = 1;
					}
					else{
						output_ann1[currentsize][k] = 0;
					}
				}
				++currentsize;
				//resize outputs if necessary
				if(currentsize == maxsize){
					maxsize = maxsize *2;
					output_ann1 = realloc(output_ann1, maxsize*sizeof(float *));					
				}
			}
		}
	}

	input_ann2 = resample(output_ann1, num_output_ann1, currentsize, ann2->num_input);

	for(i=0; i<ann2->num_input; i+=4){
		printf("%f %f %f %f\n",input_ann2[i], input_ann2[i+1], input_ann2[i+2], input_ann2[i+3]);
	}
	printf("\n\n");

	output_ann2 = fann_run(ann2, input_ann2);
	high = FLT_MIN;
	high_index = 0;
	for(k=0; k<ann2->num_output; k++){
		if(output_ann2[k]>high){
			high = output_ann2[k];
			high_index =k;
		}
	}

	for(i=0; i<ann2->num_output; i++){
		printf("%f ",output_ann2[i]);
	}
	printf("\n");

	printf("I think that you did: %s\n", gestures[high_index]);
	return 1;
}

int main(int argc, char * argv[]){
	char * path_nn1; //path to first layer of network
	char * path_nn2; //path to second layer
	int fd_pipe; //file descriptor to wii data pipe
	struct fann *ann1;
	struct fann *ann2;
	int inputs_ann1, inputs_ann2;
	pid_t wiimote;

	if(argc != 3){
		printf("Run the program as follows:\n"
			   "wiigesturedata <Path_to_network1> <Path_to_network2>\n");
		return -1;
	}
	if((access(argv[1], F_OK)==-1) || (access(argv[2], F_OK)==-1)){
		printf("Please specify valid paths to the networks.\n");
		return -2;
	}
	ann1 = fann_create_from_file(argv[1]);
	ann2 = fann_create_from_file(argv[2]);
	
	//setup pipe to wiidata
	fd_pipe =open(PATH_TO_WIIDATA, 0666);
	
	if((wiimote = launch_wii_process()) <0){
		return 1;
	}
		
	if(wait_on_signal(fd_pipe)){
		printf("Wiimote detected successfully\n");
	}else{
		printf("No wiimotes detected\n");
		return -3;
	}
	
//	signal(SIGINT, signal_handler);
	while(keepgoing){
		process(ann1, ann2, fd_pipe);
	}
	printf("Killing wiimote process\n");
	kill(wiimote, SIGINT);
	return 0;
}
