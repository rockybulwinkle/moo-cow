#include "fann.h"

int main()
{
    const unsigned int num_input = 6;
    const unsigned int num_output = 4;
    const unsigned int num_layers = 4;
    const unsigned int num_neurons_hidden = 200;
    const float desired_error = (const float) 0.006;
    const unsigned int max_epochs = 500000;
    const unsigned int epochs_between_reports = 10;

    //struct fann *ann = fann_create_standard(num_layers, num_input,
    //    num_neurons_hidden, num_output);
    unsigned int layers []= {6, 100, 100, 4};
    struct fann * ann = fann_create_standard_array(4, layers);

    //fann_set_learning_rate(ann, .001);

    //fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 1);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 2);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 3);
    fann_train_on_file(ann, "../training_file", max_epochs,
        epochs_between_reports, desired_error);

    fann_save(ann, "wii.net");

    fann_destroy(ann);

    return 0;
}
