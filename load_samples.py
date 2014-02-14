#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:

from pybrain.datasets            import SequentialDataSet
import globals

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

def load_samples(filter_key = None):
    
    sample_classes = dict()
    classes = sorted(get_sample_directories(globals.SIMPLE_SAMPLES_DIR)) #get list of motion sample directories
    if filter is not None:
        classes = filter(filter_key, classes)

    for directory in classes:
        sample_classes[directory] = [] #create entry in dictionary
        for sample in get_files_in_directory(directory):  #get sample files for that motion sample
            sample_classes[directory].append(load_file(sample, directory))
    return sample_classes

def load_file(sample_file, class_):
    samples = []
    with open(sample_file, "r") as f:
        for line in f:
            line = line.split()
            line = map(float, line)
            samples.append(line)
    return samples

def load_sequential_training_set(filter_key = None):
    samples = load_samples(filter_key)
    num_classes = len(samples.keys())
    ds = SequentialDataSet(globals.NUM_INPUTS, num_classes)

    for idx,key in enumerate(sorted(samples.keys())):
        output = [0]*num_classes
        output[idx] = 1
        for sample in samples[key]:
            ds.newSequence()
            for point in sample:
                ds.appendLinked(tuple(point), output)

    return ds

        
