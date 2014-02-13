#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:

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

def load_samples():
    sample_classes = dict()
    for directory in sorted(get_sample_directories(globals.SAMPLES_DIR)): #get list of motion sample directories
        sample_classes[directory] = [] #create entry in dictionary
        for sample in get_files_in_directory(directory):  #get sample files for that motion sample
            sample_classes[directory].append(load_file(sample, directory))
    return sample_classes

def load_file(sample_file, class_):
    samples = []
    with open(sample_file, "r") as f:
        for line in f:
            line = line.split(",")
            line = map(float, line)
            samples.append(line)
    return samples
 
