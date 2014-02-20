#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <float.h>

#include "fann.h"
#include "resample.h"

#define PATH_TO_WIIDATA "wiidata"

char  gestures[8][30] = {"ccwcircle","cwcircle","down","left","leftrightleft", "right", "rightleftright", "up"}; 


void process(struct fann *ann1, struct fann *ann2, int fd){
	char data[100];
	int i, j, k;
	char * token;
	int maxsize = 1;
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
	float gyro[]={0,0,0};

	while(!gesture_complete){
		if(read(fd,data,100)!=0){ //read from pipe
			i=0;
			token = strtok(data, " ");
			while(token != NULL){ //save to input[]
				if(!strcmp(token,"stop")){
					gesture_complete = 1; //check for stop signal if gesture is complete
					break;
				}else{
					input_ann1[i++] = atof(token);
					token = strtok(NULL, " ");
				}
			}
			if(!gesture_complete){
				//Run input through first layer
				temp_output = fann_run(ann1, input_ann1);
				gyro[0] += input_ann1[3];
				gyro[1] += input_ann1[4];
				gyro[2] += input_ann1[5];
				output_ann1[currentsize] = (float *) malloc((num_output_ann1+3)*sizeof(float));
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
				output_ann1[currentsize][num_output_ann1] = gyro[0];
				output_ann1[currentsize][num_output_ann1+1] = gyro[1];
				output_ann1[currentsize][num_output_ann1+2] = gyro[2];
				++currentsize;
				//resize outputs if necessary
				if(currentsize == maxsize){
					maxsize = maxsize *2;
					output_ann1 = realloc(output_ann1, maxsize*sizeof(float *));					
				}
			}
		}
	}

	input_ann2 = resample(output_ann1, num_output_ann1+3, currentsize, ann2->num_input);
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
	
	//free data
	for(i=0; i< currentsize; i++){
		free(output_ann1[i]);
	}
	free(output_ann1);
}

int main(int argc, char * argv[]){
	char * path_nn1; //path to first layer of network
	char * path_nn2; //path to second layer
	int fd_pipe; //file descriptor to wii data pipe
	struct fann *ann1;
	struct fann *ann2;
	int inputs_ann1, inputs_ann2;

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
	
	while(1){
		process(ann1, ann2, fd_pipe);
	}

}
