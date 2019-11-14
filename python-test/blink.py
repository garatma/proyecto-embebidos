# -*- coding: utf-8 -*-

import serial
import time

seguir = True

ser = serial.Serial("/dev/ttyACM0", 9600)

while seguir:
    try:
        entrada = int(input("Op (1/0): "))

        if (entrada == 2):
            ser.close()
            seguir = False
        else:
            ser.write(str.encode(str(entrada)))

    except ValueError:
        print("No es un número")

