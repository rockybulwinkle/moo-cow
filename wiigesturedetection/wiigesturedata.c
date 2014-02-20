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

char  gestures[6][30] = {"down","left","lrl", "right", "rlr", "up"}; 
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

int find_index(int n, float * temp){
	float high = FLT_MIN;
	int high_index = 0;
	int i;
	for(i=0; i<n; i++){
		if(temp[i]>high){
			high = temp[i];
			high_index = i;
		}
	}
	return high_index;
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

//	for(i=0; i<ann2->num_input; i+=4){
//		printf("%f %f %f %f\n",input_ann2[i], input_ann2[i+1], input_ann2[i+2], input_ann2[i+3]);
//	}
//	printf("\n\n");

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

int count_files(char * path){
	char command[100];
	char size[10];
	FILE * ls;
	int num_files;
	
	sprintf(command, "ls -l %s | wc -l", path);
	ls =popen(command, "r");
	fread(&size, sizeof(char), 10, ls);
	num_files = atoi(size)-1;
	pclose(ls);
//	printf("Number of test files: %d\n", num_files);
	return num_files;
}

void process_test_data(char * dir_path, struct fann *ann1, struct fann *ann2){
	int i, j, high_index;
	int results[6] = {0,0,0,0,0,0};
	float high;
	int maxsize = 1;
	int currentsize = 0;
	FILE * test;
	char * token;
	char test_path[100];
	char data[100];
	int num_files = count_files(dir_path);
	float ann1_input[ann1->num_input];
	float ** ann1_output;
	float * ann2_input;
	float * ann2_output;
	float * temp_output;

	ann1_output = (float**) malloc(maxsize*sizeof(float *));

	for(i=0; i<num_files; i++){
		currentsize=0;
		sprintf(test_path, "%s%d" ,dir_path, i);
		test = fopen(test_path, "r");
		while(fscanf(test, "%[^\n]%*c",data) !=EOF){
			j=0;
//			printf("%s\n", data);
			token = strtok(data, " ");
			while(token != NULL){
				ann1_input[j++] = atof(token);
				token = strtok(NULL, " ");
			}
			temp_output = fann_run(ann1, ann1_input);
			ann1_output[currentsize] = (float *) malloc(ann1->num_output*sizeof(float));

			high_index =find_index(ann1->num_output, temp_output);

			for(j=0; j<ann1->num_output; j++){
				if(j == high_index){
					ann1_output[currentsize][j] = 1;
				}else{
					ann1_output[currentsize][j] = 0;
				}
			}

			++currentsize;
			if(currentsize == maxsize){
				maxsize = maxsize*2;
				ann1_output = realloc(ann1_output, maxsize*sizeof(float *));
			}
		}
		ann2_input = resample(ann1_output, ann1->num_output, currentsize, ann2->num_input);
		ann2_output = fann_run(ann2, ann2_input);

//		for(j=0; j<ann2->num_output; j++){
//			printf("%f ", ann2_output[j]);
//		}
//		printf("\n");

		high = FLT_MIN;
		high_index = 0;
		for(j=0; j<ann2->num_output; j++){
			if(ann2_output[j]>high){
				high = ann2_output[j];
				high_index =j;
			}	
		}
		++results[high_index];
	}
	for(i=0; i<6; i++){
		printf("%s:\t\t\t%d\n", gestures[i], results[i]);
	}
	printf("\nTotal files:\t\t%d\n\n\n", num_files);
}

int main(int argc, char * argv[]){
	if (argc == 4){
		printf("%s\n", argv[3]);
	}
	char * path_nn1; //path to first layer of network
	char * path_nn2; //path to second layer
	int fd_pipe; //file descriptor to wii data pipe
	struct fann *ann1;
	struct fann *ann2;
	int inputs_ann1, inputs_ann2;
	pid_t wiimote;
	char path_to_test[100];

//	printf("%d\n\n", argc);
	if(argc != 3 && argc !=4){
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
	
	if(argc !=4){
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
	else{
		if((access(argv[3], F_OK)==-1)){
			printf("Please specify a valid path to the test data\n");
			return -2;
		}
		process_test_data(argv[3], ann1, ann2);
	}
}
