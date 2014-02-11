#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:


import subprocess
import os
import atexit
import time
import build_net
from pybrain.datasets            import SequentialDataSet

WII_DATA_FILENAME = "wiidata"
WII_CAPTURE = "wiiuse/wii_capture"

def signal_handler():
    print "exiting"
    close_c_code()
    exit(0)

def mkfifo():
    os.system("mkfifo %s"%WII_DATA_FILENAME)

def rmfifo():
    os.system("rm -f %s"%WII_DATA_FILENAME)

def launch_c_code():
    mkfifo()
    os.system(WII_CAPTURE + " 2>&1 1>/dev/null&")

def close_c_code():
    os.system("kill `pidof %s`"%WII_CAPTURE)
    rmfifo()

def get_messages():
    with open(WII_DATA_FILENAME,"r") as f:
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
                samples.newSequence()
            elif msg[0] == "stop":
                print "done recording data, waiting"
                if num_samples > 10:
                    done = True
                num_samples += 1
            elif len(msg) > 1:
                print msg
                data = map(float, msg)[0:6]
                samples.appendLinked(tuple(data), (0,1) if num_samples < 5 else (1,0))

    return samples

def test_net(net):
    while True:
        for msg in get_messages():

            if msg[0] is "start":
                print "recording data"
            elif msg[0] is "stop":
                print "done recording data, waiting"
            elif len(msg) > 1:
                data = map(float, msg)[0:6]
                net.activate(data)

   

if __name__=="__main__":
    atexit.register(signal_handler)
    launch_c_code() 
    net = build_net.build_network(6, 10, 2)
    samples = get_samples()
    print "training"
    build_net.train(net, samples)
    print "done training, now enter test data"
    test_net(net)
    close_c_code()
