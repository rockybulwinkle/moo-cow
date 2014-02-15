#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "fann.h"

#define PATH_TO_WII_PIPE "wiiuse/wiidata"
#define PATH_TO_NETWORK "fann/wii.net"
#define NUM_INPUTS 6
#define NUM_OUTPUTS 4

int main(int argc, char * args[]){
    char data[50]; //Arbitrary length
    char * token;
    FANN_EXTERNAL fann_type input[NUM_INPUTS];
    FANN_EXTERNAL fann_type * output;
    int i;
    int fd = open(PATH_TO_WII_PIPE, 0666);
    struct fann *ann = fann_create_from_file(PATH_TO_NETWORK);

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
}
