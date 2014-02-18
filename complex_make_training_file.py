#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:
"""
this file creates a training file for FANN based on the data created by wiiuse/capture
"""
import load_samples
import os

samples = load_samples.load_samples(get_file_name_only = True, input_directory="wiiuse/samples/simple/")
new_samples = dict()
for key in sorted(samples.keys()):
    joined_samples = []
    for sample in samples[key]:
        for point in sample:
            joined_samples.append(point)
    new_samples[key] = joined_samples

num_samples = sum(map(lambda x: len(new_samples[x]), new_samples)) #get number of samples
print "total number of samples: %d"%num_samples

output_values = ["1 0", "0 1"]

f = open("training_file", "w")

f.write("%d 4 %d\n"%(num_samples, len(output_values)))

for idx, key in enumerate(sorted(new_samples.keys())):
    for sample in  new_samples[key]:
        with open(sample, "r") as input_file:
            input_length = 0
            for line in input_file:
                input_length += 1

        os.system("./resample %s %d %s %d"%(sample, input_length, "/tmp/wii_out_file", 100))
        with open("/tmp/wii_out_file", "r") as tmp_file:
            f.write(" ".join(map(str, list(tmp_file))).replace("\n", ""))
        #f.write(" ".join(map(str, sample)))
        f.write("\n")
        f.write(output_values[idx])
        f.write("\n")
f.close()
