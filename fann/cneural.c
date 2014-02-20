#include "fann.h"

int main()
{
    const float desired_error = (const float) 0.01;
    const unsigned int max_epochs = 50000;
    const unsigned int epochs_between_reports = 10;

    unsigned int layers []= {6, 200, 200, 4};
    struct fann * ann = fann_create_standard_array(4, layers);

    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 1);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 2);
    fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 3);
    fann_train_on_file(ann, "../training_file", max_epochs,
        epochs_between_reports, desired_error);

    fann_save(ann, "wii.net");

    fann_destroy(ann);

    return 0;
}

