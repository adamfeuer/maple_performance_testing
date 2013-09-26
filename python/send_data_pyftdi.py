#!/usr/bin/env python

import sys, serial, time, datetime
import pyftdi.serialext

LINES = 1000
LINE_LENGTH = 3000
argsLen = len(sys.argv)
if argsLen < 2 or argsLen > 3:
   print "Usage: send_test [serial device name] [baudrate]"
   sys.exit
PORT = sys.argv[1]
BAUDRATE = sys.argv[2]

chars = ""
ser = serial.serial_for_url(PORT, baudrate=BAUDRATE, timeout=0)

print "using device: %s" % ser.portstr
line = "x" * LINE_LENGTH
count = 0
start = datetime.datetime.today()
for outer in xrange(0, LINES):
   ser.write(line)
   count += len(line)
end = datetime.datetime.today()
delta = end - start
deltaSeconds = delta.total_seconds()
bytesPerSecond = count / deltaSeconds 
print "%d bytes transmitted in %f seconds." % (count, deltaSeconds)
print "Bytes per second: %f" % bytesPerSecond
ser.close()


