#include <samplerate.h>
#include "resample.h"
#include <stdlib.h>
#include <stdio.h>
/*
input is a two dimensional array, first dimension the sample number, second dimension is the size of the output of the first layer of the nn

input_width is the number of output of the first layer

num_samples is then length of the gesture

num_output is the number of inputs to the second neural network
*/
float * resample(float ** input, int input_width, int num_samples, int num_output){
	SRC_DATA data;
	data.data_out = malloc(sizeof(float)*num_output);
	data.data_in = malloc(sizeof(float) * input_width * num_samples);

	int sample_num;
	int feature;
	int i = 0;
	for (sample_num = 0; sample_num < num_samples; sample_num++){
		for (feature = 0; feature < input_width; feature++){
			data.data_in[i] = input[sample_num][feature];
			i++;
		}
	}

	data.input_frames = num_samples;
	data.output_frames = num_output/input_width;
	data.src_ratio = ((double) data.output_frames)/((double)data.input_frames);

	int retval = src_simple(&data, 4, input_width);
	if (retval){
		printf("retval: %d\n", retval);
		printf("%s\n", src_strerror(retval));
		printf("frames used:%d\n frames generated:%d", data.input_frames_used, data.output_frames_gen);
	}
	
	free(data.data_in);
	return data.data_out;
}

