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
    samples = SequentialDataSet(6, 2)
    num_samples = 0
    done = False
    while not done:
        for msg in get_messages():
            if msg[0] == "start":
                print "recording data"
                for line in get_messages():
                    pass
                samples.newSequence()
            elif msg[0] == "stop":
                print "done recording data, waiting"
                if not num_samples < 20:
                    done = True
                num_samples += 1
                print "class is %s"%"(0,1)" if num_samples < 10 else "(1,0)"
            elif len(msg) > 1:
                print msg
                data = map(float, msg)[0:6]
                samples.appendLinked(tuple(data), (0,1) if num_samples < 10 else (1,0))
    return samples



def test_net(net):
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
                if out[0] < out[1]:
                    print "left"
                else:
                    print "right"


if __name__=="__main__":
    startup.launch_c_code() 
    net = build_net.build_network(6, 5, 2)
    samples = get_samples()
    print "training"
    print samples
    build_net.train(net, samples)
    print "done training, now enter test data"
    test_net(net)
    startup.close_c_code()
    #print load_samples()

