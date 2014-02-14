#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:


import subprocess
import os
import atexit
import time
import build_net
from pybrain.datasets            import SequentialDataSet
import globals
import startup
import load_samples
import pickle
def get_messages():
    with open(globals.WII_DATA_FILENAME,"r") as f:
        for line in f:
            if line=='\x00':
                continue
            msg = line.strip().replace("\x00", "").split()
            yield msg

def get_samples():
    samples = SequentialDataSet(6, 4)
    num_samples = 1
    done = False
    class_ = 0
    class_output = [1, 0, 0, 0]
    while not done:
        for msg in get_messages():
            if msg[0] == "start":
                print "recording data"
                for line in get_messages():
                    pass
                samples.newSequence()
                class_output = [0]*4
                class_output[class_] = 1
                print "hi"

            elif msg[0] == "stop":
                print "done recording data, waiting"
                if num_samples % 10 == 0:
                    class_ += 1
                if class_ == 4:
                    done = True
                num_samples += 1
                print "class is %s"%str(class_output)

            elif len(msg) > 1:
                print msg
                data = map(float, msg)[0:6]
                print class_output
                samples.appendLinked(tuple(data), class_output)
    return samples



def test_net(net):
    directions = sorted(load_samples.load_samples().keys())
    while True:
        for msg in get_messages():
            if msg[0] == "start":
                for line in get_messages():
                    pass
                print "resetting"
                net.reset()
            elif len(msg) > 1:
                data = map(float, msg)
                out =  net.activate(data)
                print out
                out = zip(out, list(range(len(directions))))
                print directions[max(out, key=lambda x: x[0])[1]]

def left_right_filter(x):
    return "left" in x or "right" in x
def up_down_filter(x):
    return "up" in x or "down" in x

if __name__=="__main__":
    startup.launch_c_code()
    up_down_net = build_net.build_network(globals.NUM_INPUTS, 40, 4)

    up_down_samples = load_samples.load_sequential_training_set()

    print "training"
    build_net.train(up_down_net, up_down_samples)
    #with open("trained_net", "r") as f:
    #    up_down_net = pickle.load(f)
    print "done training, now enter test data"
    test_net(up_down_net)
    startup.close_c_code()

