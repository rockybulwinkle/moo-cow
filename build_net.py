#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:

from pybrain.structure import RecurrentNetwork
from pybrain.structure import LinearLayer
from pybrain.structure import SigmoidLayer
from pybrain.structure import TanhLayer
from pybrain.structure import FullConnection
from pybrain.datasets            import SequentialDataSet
from pybrain.utilities           import percentError
from pybrain.supervised.trainers import BackpropTrainer
from pybrain.structure.modules   import SoftmaxLayer
from pybrain.tools.shortcuts import buildNetwork
import random
import signal
import sys
import pickle

def build_network(num_inputs, num_hidden, num_output):
    n = RecurrentNetwork() #create network
    n.addInputModule(LinearLayer(num_inputs, name="in")) #add input layer
    n.addModule(SigmoidLayer(num_hidden, name="hidden")) #add hidden layer
    n.addModule(TanhLayer(num_hidden, name="hidden2")) #add hidden layer
    n.addOutputModule(SigmoidLayer(num_output, name="out")) #add output layer

    n.addConnection(FullConnection(n["in"], n["hidden"])) #connect the input to the hidden
    n.addConnection(FullConnection(n["hidden"], n["hidden2"])) #connect the input to the hidden
    n.addConnection(FullConnection(n["hidden2"], n["out"])) #connect the hidden to the output
    n.addRecurrentConnection(FullConnection(n["out"], n["hidden"], name="recur")) #connect output of current hidden layer to input of the hidden layer for the next activation
    n.addRecurrentConnection(FullConnection(n["hidden"], n["in"], name="recur2")) #connect output of current hidden layer to input of the hidden layer for the next activation
    n.sortModules() #done!
    

    return n
def get_set(direction):
    i = [random.random()]
    for _ in range(20):
        i.append(direction*random.random()+i[-1])
    return i
    old_min = min(i)
    old_range = max(i) - old_min
    new_min = 0
    new_range = 10 + 0.9999999999 - new_min
    output = [int((n - old_min) / old_range * new_range + new_min) for n in i]
    return output


done = False
def train(net, ds):
    global done
    def signal_handler(signal, frame):
        global done
        done = True
    signal.signal(signal.SIGINT, signal_handler)
    net.randomize()
    trainer = BackpropTrainer(net, ds, learningrate=.0001, batchlearning=True, lrdecay=.99)
    for i in range(1000):
        x =  trainer.train()
        print "%d: %f"%(i,x)
        if done:
            break
    with open("trained_net", "w") as f:
        pickle.dump(net, f)


