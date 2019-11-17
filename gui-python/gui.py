# -*- coding: utf-8-*-
from appJar import gui
import serial
import time
import os

# defining the gui
app = gui("Effect unit controller", "700x540") 
app.setFont(18)

# defining the serial variable
ser = None
serial_tested = False

def port_test(btn):
    global ser
    try:
        ser = serial.Serial(app.getEntry("Port device"), 9600)
        app.okBox("Port test", "Port confirmed.", parent=None)
    except serial.serialutil.SerialException:
        app.okBox("Port test", "Can't access specified port.", parent=None)

# field to enter port device and button to test connectivity
app.addLabelEntry("Port device",0,0)
if (os.name in "posix"):
    app.setEntry("Port device", "/dev/ttyACM0", callFunction=port_test)
else:
    app.setEntry("Port device", "COM0", callFunction=port_test)
app.addButton("Confirm port",port_test,0,1)

# every effect
app.addRadioButton("Effect", "Bit Crusher")
app.addRadioButton("Effect", "Booster")
app.addRadioButton("Effect", "Daft Punk Octaver")
app.addRadioButton("Effect", "Delay")
app.addRadioButton("Effect", "Distortion")
app.addRadioButton("Effect", "Fuzz")
app.addRadioButton("Effect", "Metronome")
app.addRadioButton("Effect", "Signal Generator")
app.addRadioButton("Effect", "Tremolo")
app.addRadioButton("Effect", "Clean")

# values for respective scales: [name, min, max, current]
scales = [ ["Bit shift",        0, 16, 8],
           ["Volume",           0, 32768, 16384],
           ["Octave pitch",     0, 500, 250],
           ["Delay time",       0, 2000, 1000],
           ["Distortion",       0, 32768, 16384],
           ["Fuzz",             0, 32768, 16384],
           ["BPM",              0, 300, 150],
           ["Frecuency",        0, 1024, 512],
           ["Modulation speed", 0, 1024, 512],
           ["---", 0, 0, 0]
         ]

# effect dictionary (to index scales array)
effect_dictionary = {
        "Bit Crusher": 0,
        "Booster": 1,
        "Daft Punk Octaver": 2,
        "Delay": 3,
        "Distortion": 4,
        "Fuzz": 5,
        "Metronome": 6,
        "Signal Generator": 7,
        "Tremolo": 8,
        "Clean": 9
}

app.addHorizontalSeparator(colour="black", colspan=2)

# initially, bit shifter is selected
app.addLabel("Effect label", "Bit shift", colspan=2)
app.addScale("Effect scale", colspan=2)
app.setFont("Effect scale")
app.setScaleRange("Effect scale", scales[0][1], scales[0][2], scales[0][3])
app.showScaleIntervals("Effect scale", scales[0][2]/4)
app.showScaleValue("Effect scale", show=True)

# (index of) effect currently selected
effect_number = 0

# changes the label, scale and its attributes because a new effect was selected
def change_effect(radio_button):
    global effect_number
    effect_number = effect_dictionary[app.getRadioButton("Effect")]
    app.setLabel("Effect label", scales[effect_number][0])
    app.setScaleRange("Effect scale",
                      scales[effect_number][1],
                      scales[effect_number][2],
                      curr=scales[effect_number][3])
    app.showScaleIntervals("Effect scale", scales[effect_number][2]/4)
    app.setScale("Effect scale", scales[effect_number][3], callFunction=False)
    if (ser != None):
        ser.write(str.encode('e'))
        ser.write(str.encode(str(effect_number)))
        print(ser.read())

# changes the scale array current value because the scale was moved
def change_current_value(scale):
    scales[effect_number][3] = app.getScale("Effect scale")
    if (ser != None):
        ser.write(str.encode(str(scales[effect_number][3])))
        print(ser.read())

# set callbacks
app.setScaleChangeFunction("Effect scale", change_current_value)
app.setRadioButtonChangeFunction("Effect", change_effect)

app.go()
