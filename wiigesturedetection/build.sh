#!/usr/bin/env bash
gcc wiigesturedata.c -lfann -lm resample.c -lsamplerate -o moo-cow
gcc wii_capture.c -lm -lbluetooth -lwiiuse libwiiuse.a -o cow_capture  
