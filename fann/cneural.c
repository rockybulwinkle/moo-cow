#include "fann.h"
#include <math.h>

#define NUM_INPUTS 6
#define NUM_OUTPUTS 4
#define NUM_LAYERS 3
#define NUM_NEURONS 3

int main()
{

    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 500000;
    const unsigned int epochs_between_reports = 1000;

    struct fann *ann = fann_create_standard(NUM_LAYERS, NUM_INPUTS,
        NUM_NEURONS, NUM_OUTPUTS);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_train_on_file(ann, "xor.data", max_epochs,
        epochs_between_reports, desired_error);

    fann_save(ann, "xor_float.net");

    fann_destroy(ann);

    return 0;
}
