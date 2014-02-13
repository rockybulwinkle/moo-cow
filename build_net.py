#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:

from pybrain.structure import RecurrentNetwork
from pybrain.structure import LinearLayer
from pybrain.structure import SigmoidLayer
from pybrain.structure import FullConnection
from pybrain.datasets            import SequentialDataSet
from pybrain.utilities           import percentError
from pybrain.supervised.trainers import BackpropTrainer
from pybrain.structure.modules   import SoftmaxLayer
import random
def build_network(num_inputs, num_hidden, num_output):
    n = RecurrentNetwork() #create network
    n.addInputModule(LinearLayer(num_inputs, name="in")) #add input layer
    n.addModule(SigmoidLayer(num_hidden, name="hidden")) #add hidden layer
    n.addOutputModule(LinearLayer(num_output, name="out")) #add output layer
    n.addConnection(FullConnection(n["in"], n["hidden"])) #connect the input to the hidden
    n.addConnection(FullConnection(n["hidden"], n["out"])) #connect the hidden to the output
    n.addRecurrentConnection(FullConnection(n["out"], n["hidden"], name="recur")) #connect output of current hidden layer to input of the hidden layer for the next activation
    
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

def train(net, ds):
    net.randomize()
    trainer = BackpropTrainer(net, ds, learningrate=.001, batchlearning=True)

    for _ in range(500):
        x =  trainer.train()
        print x














