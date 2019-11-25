# -*- coding: utf-8-*-
from appJar import gui
import serial
import time
import os

# defining the gui
app = gui("Effect unit controller", "600x370") 
app.setFg("white", override=False)
app.setIcon("icon.gif")
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
app.addRadioButton("Effect", "Daft Punk Octaver")
app.addRadioButton("Effect", "Distortion")
app.addRadioButton("Effect", "Fuzz")
app.addRadioButton("Effect", "Clean")

# values for respective scales: [name, min, max, current+1]
# the '+1' is used to differentiate zero (new effect message) from effect
# modifier value=0. the arduino must decrease the modifier by one.
scales = [ [None,               None,    None,    None],
           ["Bit shift",        "00001", "00017", "00001"],
           ["Octave pitch",     "00001", "00501", "00001"],
           ["Distortion",       "00001", "32769", "00001"],
           ["Fuzz",             "00001", "32769", "00001"],
           ["No modifier",      "00001", "00001", "00001"]
]

# effect dictionary (to index scales array)
effect_dictionary = {
        "Bit Crusher":          "00001",
        "Daft Punk Octaver":    "00002",
        "Distortion":           "00003",
        "Fuzz":                 "00004",
        "Clean":                "00005"
}

effect_number = 1
change_scale = True

app.addHorizontalSeparator(colour="black", colspan=2)

# initially, bit shifter is selected
app.addLabel("Effect label", scales[effect_number][0], colspan=2)
app.addScale("Effect scale", colspan=2)
app.setFont("Effect scale")
app.setScaleRange("Effect scale", int(scales[effect_number][1])-1,
                                  int(scales[effect_number][2])-1, 
                                  int(scales[effect_number][3])-1)
app.showScaleIntervals("Effect scale",
                       (int(scales[effect_number][2])-1)/4)
app.showScaleValue("Effect scale", show=True)

def digit_count(number):
    digits = 0
    backup = number
    while (backup > 0):
        backup = backup // 10
        digits = digits + 1
    return digits

# changes the scale array current value because the scale was moved
def change_current_value(scale):
    if (change_scale):
        number = app.getScale("Effect scale")+1
        digits = digit_count(number)
        counter = 0
        new_number = ''
        to_send = ''
        for counter in range(0, 5-digits):
            new_number += '0'
        if (number != 0):
            new_number += str(number)
        scales[effect_number][3] = new_number
        if (ser != None and change_scale):
            ser.write(str.encode(new_number))

# changes the label, scale and its attributes because a new effect was selected
def change_effect(radio_button):
    global effect_number, change_scale
    effect_number_string = effect_dictionary[app.getRadioButton("Effect")]
    effect_number = int(effect_number_string)
    app.setLabel("Effect label", scales[effect_number][0])
    if (ser != None):
        ser.write(str.encode('00000'))
        ser.write(str.encode(effect_number_string))
    change_scale = False
    app.setScaleRange("Effect scale",
                      int(scales[effect_number][1])-1,
                      int(scales[effect_number][2])-1,
                      curr=int(scales[effect_number][3])-1)
    app.showScaleIntervals("Effect scale",
                           (int(scales[effect_number][2])-1)/4)
    app.setScale("Effect scale", int(scales[effect_number][3])-1, callFunction=False)
    change_scale = True
    change_current_value(None)

# set callbacks
app.setScaleChangeFunction("Effect scale", change_current_value)
app.setRadioButtonChangeFunction("Effect", change_effect)

app.go()
