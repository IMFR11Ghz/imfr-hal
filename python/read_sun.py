import serial
import sys
ser = serial.Serial('/dev/ttyUSB0', 115200 ,timeout=1)
with open('radio_sun.log', 'w+') as sun:
    while True:
        data = str(ser.readline(),'utf-8')
        sun.write(data)
        sun.flush()
