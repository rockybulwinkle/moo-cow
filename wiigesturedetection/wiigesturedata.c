#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fann.h"

#define PATH_TO_WIIDATA "wiidata";

int * processInputs(struct fann *ann1, struct fann *ann2, int fd){
	char data[100];
	int i;
	char * token;
	int input[inputs_ann1];
	if(read(fd,data,100)!=0){
		i=0;
		token = strtok(data, " ");
		while(token != NULL){
			input(i++)
		}
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
	if((access(argv[1], F_OK)==-1) || (access(argv[3], F_OK)==-1)){
		printf("Please specify valid paths to the networks.\n");
		return -2;
	}
	ann1 = fann_create_from_file(argv[1]);
	ann2 = fann_create_from_file(argv[2]);
	
	//setup pipe to wiidata
	fd =open(PATH_TO_WIIDATA, 0666);
	
	while(1){
		if(read(fd,data,50)!=0){
			i=0;
			
		}
	}

}
