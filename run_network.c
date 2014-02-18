#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "fann.h"

#define PATH_TO_WII_PIPE "wiiuse/wiidata"
#define PATH_TO_NETWORK "fann/wii.net"
#define PATH_TO_SAMPLES "wiiuse/samples/advanced/"
#define PATH_TO_NEW_SAMPLES "samples/"
#define NUM_INPUTS 6
#define NUM_OUTPUTS 4

int main(int argc, char * args[]){
    char data[50]; //Arbitrary length
	char path[100];
    char * token;
    FANN_EXTERNAL fann_type input[NUM_INPUTS];
    FANN_EXTERNAL fann_type * output;
    int num_samples, i;
    
    if(argc==2){
        
        char command[100] = "ls -l ";
        char size[10];
        FILE * ls;
        sprintf(path, "%s%s/", PATH_TO_SAMPLES, args[1]);
        strcat(command, path);
        strcat(command, " | wc -l");

        ls = popen(command, "r");
        fread(&size, sizeof(char), 10, ls);
        num_samples = atoi(size)-1;
        pclose(ls);
        printf("Sample count: %d\n", num_samples);
    }

    int fd = open(PATH_TO_WII_PIPE, 0666);
    struct fann *ann = fann_create_from_file(PATH_TO_NETWORK);


	if(argc==1){
	    while(1){
	        if(read(fd,data, 50)!=0){ //Read from pipe
        	    i=0;
    	        token = strtok(data, " "); //Break up data stream
	            while(token != NULL){
        	        input[i++] = atof(token);
    	            token = strtok(NULL, " ");
	            }
            	output = fann_run(ann, input); //run input through network
        	    for(i=0; i<NUM_OUTPUTS; i++){
    	            printf("%f ",output[i]);
	            }
            	printf("\n");
			}
		}
	}else{
		FILE * sample;
		char filePath[100];
		char message[100];
		for(i=0; i<num_samples; i++){
			sprintf(filePath, "%s%d", path, i);
			printf("%s\n",filePath);	
			sample = fopen(filePath, "r");
			fread(message, sizeof(char), 100, sample);
			fclose(sample);	

			sprintf(filePath, "%s%d",PATH_TO_NEW_SAMPLES,i);
			sample = fopen(filePath, "w+");
			i=0;
    	    token = strtok(message, " "); //Break up data stream
	        while(token != NULL){
        		input[i++] = atof(token);
    	        token = strtok(NULL, " ");
	        }
            output = fann_run(ann, input); //run input through network
            for(i=0; i<NUM_OUTPUTS; i++){
				fprintf(sample, "%d ", i);
    	        printf("%f ",output[i]);
	        }
			fprintf(sample,"\n");
          	printf("\n");
			fclose(sample);
		}
	}
}
