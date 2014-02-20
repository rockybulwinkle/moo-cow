#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <float.h>
#include "fann.h"

#define PATH_TO_WII_PIPE "wiiuse/wiidata"
#define PATH_TO_NETWORK "fann/wii.net"
#define PATH_TO_SAMPLES "wiiuse/samples/advanced/"
#define PATH_TO_NEW_SAMPLES "samples/"
#define NUM_INPUTS 6
#define NUM_OUTPUTS 4

//const float calibration[] = {7881.3725, 8012.1775, 8354.2725};
const float calibration[] = {0,0,0};
char output_names[4][30]={"down", "left", "right","up"};
int main(int argc, char * args[]){
    char data[100]; //Arbitrary length
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
	        if(read(fd,data, 100)!=0){ //Read from pipe
        	    i=0;
    	        token = strtok(data, " "); //Break up data stream
	            while(token != NULL){
        	        input[i++] = atof(token);
    	            token = strtok(NULL, " ");
	            }
            	output = fann_run(ann, input); //run input through network
		    int max_index = 0;
		    float max_value = FLT_MIN;
        	    for(i=0; i<NUM_OUTPUTS; i++){
		        if (output[i] > max_value){
		    	    max_value = output[i];
			    max_index = i;
		        }
	            }
		    printf("%s ", output_names[max_index]);
		    for(i=0; i<NUM_OUTPUTS; i++){
    	                printf("%f ",output[i]);
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
			
			double int_gyro[3];
			while(fscanf(sample, "%[^\n]%*c", message)!=EOF){
				j=0;
				printf("%s\n", message);
	    	    token = strtok(message, " "); //message stream

		        while(token != NULL){
        			input[j] = atof(token);
//					printf("%f, %d; ", atof(token), j);
					j++;
    	    	    token = strtok(NULL, " ");
		        }
//				printf("\n");
    	        output = fann_run(ann, input); //run input through network
        	    for(k=0; k<NUM_OUTPUTS; k++){
					fprintf(newSamples, "%f ", output[k]);
    	        	//printf("%f ",output[k]);
		        }
				for(k = 3; k < 6; k++){
					int_gyro[k-3] += input[k] - calibration[k-3];
				}
				fprintf(newSamples, "%f %f %f ", int_gyro[0], int_gyro[1], int_gyro[2]);

				fprintf(newSamples, "\n");
			}
			fclose(newSamples);
			fclose(sample);	
		}
	}
}
