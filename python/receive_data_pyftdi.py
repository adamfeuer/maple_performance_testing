#!/usr/bin/env python

import sys, serial, time, datetime
import pyftdi.serialext

argsLen = len(sys.argv)
if argsLen < 2 or argsLen > 3:
   print "Usage: send_test [serial device name] [baudrate]"
   sys.exit
PORT = sys.argv[1]
BAUDRATE = sys.argv[2]

chars = ""
ser = serial.serial_for_url(PORT, baudrate=BAUDRATE, timeout=0)

print "using device: %s" % ser.portstr
line = ""
count = 0
start = datetime.datetime.today()
#for outer in xrange(0, 5):
#   line = ser.read(1000000)
#   count += len(line)
count = 5000000 
#count = 5000
line = ser.read(count)
end = datetime.datetime.today()
delta = end - start
deltaSeconds = delta.total_seconds()
bytesPerSecond = count / deltaSeconds 
print "%d bytes received in %f seconds." % (count, deltaSeconds)
print "Bytes per second: %f" % bytesPerSecond
ser.close()


