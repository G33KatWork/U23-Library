#!/usr/bin/python
from __future__ import division
import struct
import serdecode
import math

outfile = open("softscope.fifo", "w")

serdecode.resync()

while True:
    s = serdecode.read_frame()

    (speed, control, DAC, integral) = struct.unpack("=hhhi", s)

    outfile.write(struct.pack("=ffff", speed, control, DAC, integral))
    outfile.flush()

outfile.close()
