#
# read timestamps from serial port and compare them to the system time (UTC).
#
#   the timestamps come from an RTC connected to a Feather M0 (or similar Arduino-like) device,
#   which is read through the USB-connected serial port.
#
#   the Feather M0 is running a special sketch to output UTC timestamps, like this:
#       rtc: 2020-11-08T19:11:55
#
#
# note: hardcoded serial port below, you may need to set this differently!
#
# Usage:  (tested with python 3.7)
#   python read_rtc.py
#
# Author:
# -Rolf Widenfelt (Nov 2020)
#
#
# Credits:
#   some useful hints on using PySerial from this post:
#   https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial
#

import serial
from datetime import datetime, timedelta
import re


# choose serial port - your device may be different

ser = serial.Serial('/dev/cu.usbmodem1421')
ser.flushInput()

#
# read one line at a time from the device.
# the device should return a byte string like this:
#       rtc: 2020-11-08T19:11:55
#
# we expect this time to be UTC, because that is what it is compared to.
#
# we print it, along with the system time, a delta, and the minimum-delta.
#

min_delta = timedelta(seconds=100000)  # set a ridiculously big value to start the comparison

while True:
    try:
        ser_bytes = ser.readline()      # read device
        date_obj = datetime.utcnow()    # note: we get system time immediately *after* reading serial port

        ser_str = str(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))    # convert bytes to string
        print(ser_str + "  ", end='')
        print(date_obj, end='')

        if (len(ser_str) < 10) or (ser_str.split()[0] != "rtc:"):   # skip garbage line?
            print("")
            continue

        rtc_str = ser_str.split()[1]            # strip out "rtc: " part

        # convert to datetime object
        rtc_str = re.sub("T", " ", rtc_str)
        rtc_time_obj = datetime.strptime(rtc_str, '%Y-%m-%d %H:%M:%S')

        #
        # here, we arrange for an absolute delta, and separate out the sign.
        # we want negative deltas to imply that the RTC is trailing.
        #
        #   (negative deltas from datetime show up as being off by -1 day.. it's ugly)
        #
        sign = ""
        if (rtc_time_obj - date_obj) < timedelta(seconds=0):
            sign = "-"
            delta_time = date_obj - rtc_time_obj     # assume RTC is trailing
        else:
            delta_time = rtc_time_obj - date_obj     # assume RTC is leading

        print("  delta: {}{}".format(sign, delta_time), end='')   # show time delta
        if delta_time < min_delta:
            min_delta = delta_time

        print("  min: {}{}".format(sign, min_delta))    # show the "best" minimum delta

    except KeyboardInterrupt:
        print("Keyboard Interrupt")
        break
