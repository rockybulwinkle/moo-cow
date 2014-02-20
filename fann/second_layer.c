#include "fann.h"

int main()
{
    const unsigned int num_input = 700;
    const unsigned int num_output = 8;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 200;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 50000;
    const unsigned int epochs_between_reports = 10;

    unsigned int layers []= {700, 100, 100, 8};
    struct fann * ann = fann_create_standard_array(4, layers);

    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 1);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 2);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 3);
    fann_train_on_file(ann, "../complex_training_file", max_epochs,
        epochs_between_reports, desired_error);

    fann_save(ann, "second_layer_wii.net");

    fann_destroy(ann);

    return 0;
}
