/*
input is a two dimensional array, first dimension the sample number, second dimension is the size of the output of the first layer of the nn

input_width is the number of output of the first layer

num_samples is then length of the gesture

num_output is the number of inputs to the second neural network
*/
float * resample(float ** input, int input_width, int num_samples, int num_output);
