#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fann.h"

#define PATH_TO_WIIDATA "wiidata"


void process(struct fann *ann1, struct fann *ann2, int fd){
	char data[100];
	int i, j, k;
	char * token;
	int maxsize = 10;
	int currentsize=0;
	int gesture_complete =0;
	int num_input_ann1 = ann1->num_input;
	int num_output_ann1 = ann1->num_output;
	float input_ann1[num_input_ann1];
	float ** output_ann1;
	float * temp_output;
	//allocate memory for outputs
	output_ann1 = (float **) malloc(maxsize*sizeof(float *));

	while(!gesture_complete){

		if(read(fd,data,100)!=0){ //read from pipe
			i=0;
			printf("%s\n",data);
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
				output_ann1[currentsize] = (float *) malloc(num_output_ann1*sizeof(float));
				for(k=0; k<num_output_ann1; k++){
					output_ann1[currentsize][k] = temp_output[k];
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

	for(i=0; i<currentsize; i++){
		printf("Output%d: ",i);		
		for(j=0; j<num_output_ann1; j++){
			printf("%f ",output_ann1[i][j]);
		}
		printf("\n");
	}
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
