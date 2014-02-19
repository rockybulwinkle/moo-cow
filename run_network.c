#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "fann.h"

#define PATH_TO_WII_PIPE "wiiuse/wiidata"
#define PATH_TO_NETWORK "fann/wii.net"
#define PATH_TO_SECOND_LAYER "fann/second_layer_wii.net"
#define PATH_TO_SAMPLES "wiiuse/samples/advanced/"
#define PATH_TO_NEW_SAMPLES "samples/"
#define NUM_INPUTS 6
#define NUM_OUTPUTS 4
#define NUM_INPUTS_SECOND 400
#define NUM_OUTPUTS_SECOND 2


int getNumOfSamples(char * path){
	char command[100] = "ls -l ";
    char size[10];
    FILE * ls;
    strcat(command, path);
    strcat(command, " | wc -l");
    ls = popen(command, "r");
    fread(&size, sizeof(char), 10, ls);
    pclose(ls);
	return atoi(size)-1;
}

FANN_EXTERNAL fann_type * getOutput(char * data, struct fann * ann, int in){
	int	i=0;
	char * token;
	FANN_EXTERNAL fann_type input[in];
    token = strtok(data, " "); //Break up data stream
	while(token != NULL){
    	input[i++] = atof(token);
    	token = strtok(NULL, " ");
	}
    return fann_run(ann, input); //run input through network
}

int main(int argc, char * args[]){
    char data[50]; //Arbitrary length
	char path[100];
    FANN_EXTERNAL fann_type * output;
	FANN_EXTERNAL fann_type * output2;
    int num_samples, i, fd;
	struct fann * ann;
	struct fann * ann2;
    
    if(argc==2){        
        sprintf(path, "%s%s/", PATH_TO_SAMPLES, args[1]);
        num_samples = getNumOfSamples(path);
        printf("Sample count: %d\n", num_samples);
    }

    fd = open(PATH_TO_WII_PIPE, 0666);
    ann = fann_create_from_file(PATH_TO_NETWORK);
	ann2 = fann_create_from_file(PATH_TO_SECOND_LAYER);

	if(argc==1){
	    while(1){
	        if(read(fd,data, 50)!=0){ //Read from pipe
				output = getOutput(data, ann, NUM_INPUTS);
				output2 = fann_run(ann2, output);
				for(i=0; i<NUM_OUTPUTS_SECOND; i++){
					printf("%f ",output2[i]);
				}
				printf("\n");
			}
		}
	}else{
		FILE * sample;
		FILE * newSamples;
		char filePath[100];
		char filePathNew[100];
		char message[100];
		int j, k;

		for(i=0; i<num_samples; i++){
			sprintf(filePath, "%s%d", path, i);	
			sample = fopen(filePath, "r");

			sprintf(filePathNew, "%s%s/", PATH_TO_NEW_SAMPLES, args[1]);
			mkdir(filePathNew, 0755);
			sprintf(filePathNew, "%s%d", filePathNew, i);
			newSamples = fopen(filePathNew, "w+");

			while(fscanf(sample, "%s\n", message)!=EOF){
    	        output = getOutput(data, ann, NUM_INPUTS);
        	    for(k=0; k<NUM_OUTPUTS; k++){
					fprintf(newSamples, "%f ", output[k]);
		        }
				fprintf(newSamples, "\n");
			}
			fclose(newSamples);
			fclose(sample);	
		}
	}
}


