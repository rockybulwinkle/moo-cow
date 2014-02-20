#!/usr/bin/env bash
gcc wii_capture.c libwiiuse.a  -lm -lbluetooth -o wii_capture -o wii_gesture_capture
#gcc wiigesturedata.c -lm -lfann -o gesture
gcc wiigesturedata.c libwiiuse.a resample.c -lfann -lm -lsamplerate -lbluetooth
