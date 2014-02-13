#!/usr/bin/env python2
# vim:tabstop=4:shiftwidth=4:smarttab:expandtab:softtabstop=4:autoindent:
import atexit
import globals
import os
def signal_handler():
    print "exiting"
    close_c_code()
    exit(0)

def mkfifo():
    os.system("mkfifo %s"%globals.WII_DATA_FILENAME)

def rmfifo():
    os.system("rm -f %s"%globals.WII_DATA_FILENAME)

def launch_c_code():
    mkfifo()
    os.system(globals.WII_CAPTURE + " 2>&1 1>/dev/null&")

def close_c_code():
    os.system("kill `pidof %s`"%globals.WII_CAPTURE)
    rmfifo()



atexit.register(signal_handler)
