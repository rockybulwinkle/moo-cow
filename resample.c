#include <samplerate.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char * argv[]){
	/*
	argv 1: input file
	argv 2: num samples
	argv 3: output file
	argv 4: output num samples
	*/
	FILE * in_file = fopen(argv[1], "r");
	int i = 0;
	int in_size = atoi(argv[2]);
	int out_size = atoi(argv[4]);

	SRC_DATA src_data;
	src_data.data_in = malloc(sizeof(float)*in_size*7);
	
	for (i = 0; i < in_size*7; i+=7){
		fscanf(in_file, "%f %f %f %f %f %f %f \n", &src_data.data_in[i], &src_data.data_in[i+1], &src_data.data_in[i+2], &src_data.data_in[i+3], &src_data.data_in[i+4], &src_data.data_in[i+5], &src_data.data_in[i+6]);
		//printf("%f %f %f %f\n", src_data.data_in[i], src_data.data_in[i+1], src_data.data_in[i+2], src_data.data_in[i+3]);
	}
	fclose(in_file);

	src_data.data_out = malloc(sizeof(float)*out_size*7);

	src_data.input_frames = in_size;
	src_data.output_frames = out_size;
	src_data.src_ratio = out_size/((float)in_size);
	printf("%f\n", src_data.src_ratio);

	int ret_val = src_simple(&src_data, 4, 7);
	if (ret_val){
		printf("%s\n", src_strerror(ret_val));
		printf("frames used: %d frames generated: %d\n", src_data.input_frames_used, src_data.output_frames_gen);
	}
	FILE * out_file = fopen(argv[3], "w");
	for (i = 0; i < out_size * 7; i += 7){
		fprintf(out_file,"%f %f %f %f %f %f %f\n", src_data.data_out[i], src_data.data_out[i+1], src_data.data_out[i+2], src_data.data_out[i+3], src_data.data_out[i+4], src_data.data_out[i+5], src_data.data_out[i+6]);
		//printf("%f %f %f %f\n", src_data.data_out[i], src_data.data_out[i+1], src_data.data_out[i+2], src_data.data_out[i+3]);
	}

}
