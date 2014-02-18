#!/usr/bin/env bash

gcc cneural.c -lm -lfann -o cneural
gcc second_layer.c -lm -lfann -o second_layer
