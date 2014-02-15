#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:
"""
this file creates a training file for FANN based on the data created by wiiuse/capture
"""
import load_samples

samples = load_samples.load_samples()
new_samples = dict()
for key in samples.keys():
    joined_samples = []
    for sample in samples[key]:
        for point in sample:
            joined_samples.append(point)
    new_samples[key] = joined_samples

num_samples = sum(map(lambda x: len(new_samples[x]), new_samples)) #get number of samples
print "total number of samples: %d"%num_samples

output_values = ["1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 1"]

f = open("training_file", "w")
f.write("%d 6 4\n"%num_samples)

for idx, key in enumerate(new_samples):
    for sample in new_samples[key]:
        f.write(" ".join(map(str, sample)))
        f.write("\n")
        f.write(output_values[idx])
        f.write("\n")
f.close()




