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
    directions = ["left", "right", "up", "down"]
    while True:
        for msg in get_messages():
            if msg[0] == "start":
                for line in get_messages():
                    pass
                print "resetting"
                net.reset()
            elif len(msg) > 1:
                data = map(float, msg)[0:6]
                out =  net.activate(data)
                print out
                out = zip(out, [0,1,2,3])
                print directions[max(out, key=lambda x: x[1])[1]]



if __name__=="__main__":
    startup.launch_c_code() 
    net = build_net.build_network(6, 5, 4)
    samples = get_samples()
    print "training"
    print samples
    build_net.train(net, samples)
    print "done training, now enter test data"
    test_net(net)
    startup.close_c_code()
    #print load_samples()

