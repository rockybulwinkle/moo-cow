#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:

from pybrain.datasets            import SequentialDataSet

def get_sample_directories(sample_dir):
    import os
    if sample_dir[-1]!="/":
        sample_dir += "/"
    return map(lambda x: sample_dir+x,os.walk(sample_dir).next()[1])

def get_files_in_directory(directory):
    import os
    if directory[-1]!= "/":
        directory += "/"
    return map(lambda x: directory+x,os.walk(directory).next()[2])

def load_samples(input_directory = "", filter_key = None, get_file_name_only = False):
    assert input_directory!=""
    sample_classes = dict()
    classes = sorted(get_sample_directories(input_directory)) #get list of motion sample directories
    if filter is not None:
        classes = filter(filter_key, classes)

    for directory in classes:
        sample_classes[directory] = [] #create entry in dictionary
        for sample in get_files_in_directory(directory):  #get sample files for that motion sample
            if get_file_name_only:
                sample_classes[directory].append([sample])
            else:
                sample_classes[directory].append(load_file(sample))
    return sample_classes

def load_file(sample_file):
    samples = []
    print sample_file
    with open(sample_file, "r") as f:
        x = 0
        for line in f:
            #print line
            x += 1
            #if x%3 == 0:
            #    continue
            line = line.split()
            line = map(float, line)
            samples.append(line)
    return samples

