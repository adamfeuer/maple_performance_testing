#!/usr/bin/env python

import sys, serial, time, datetime

#PORT = "/dev/tty.usbmodem1d1111"
argsLen = len(sys.argv)
if argsLen < 2 or argsLen > 3:
   print "Usage: send_test [serial device name] [baudrate]"
   sys.exit
PORT = sys.argv[1]
BAUDRATE = sys.argv[2]

chars = ""
#ser = serial.Serial(port=PORT, baudrate=115600)
ser = serial.Serial(port=PORT, baudrate=BAUDRATE)
print "using device: %s" % ser.portstr
line = ""
count = 0
start = datetime.datetime.today()
for outer in xrange(0, 1000):
   line = ser.read(500)
   count += len(line)
end = datetime.datetime.today()
delta = end - start
deltaSeconds = delta.total_seconds()
bytesPerSecond = count / deltaSeconds 
print "%d bytes received in %f seconds." % (count, deltaSeconds)
print "Bytes per second: %f" % bytesPerSecond
ser.close()


