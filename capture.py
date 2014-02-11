#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:


import subprocess
import os


WII_DATA_FILENAME = "wii_data"
WII_CAPTURE = "wiiuse/wii_capture"
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


launch_c_code()
import time
time.sleep(10000000)
close_c_code()
