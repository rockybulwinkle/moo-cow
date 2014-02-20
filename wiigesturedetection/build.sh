#!/usr/bin/env bash
gcc wiigesturedata.c -lfann -lm resample.c -lsamplerate -o moo-cow
gcc wii_capture.c libwiiuse.a -lm -lbluetooth -o cow_capture  
