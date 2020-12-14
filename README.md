# Unidad Efectos UNO (Effect UNO Unit)

Effect unit (like a guitar pedal) made with an Arduino UNO, controlled by a portable PC GUI application.

# Details
The effect unit is connected to the musical instrument with a 1/4" jack cable. The Arduino UNO converts its analog input signal to digital, modifies it via software developed in C++, according to the user's input on the GUI application, developed in Python, running on the PC, which it itself is connected to the _pedal_ via serial port. The Arduino then converts the modified input back to analog and proceeds to transmit it to an amp, also connected with a 1/4" jack cable.

Here's is a picture of the PC GUI application, showcasing every programmed effect in the Arduino.
![GUI](https://raw.githubusercontent.com/garatma/unidad-efectos-uno/master/proyecto/informe/gui.png)

For more information, check out the [presentation](https://github.com/garatma/unidad-efectos-uno/raw/master/proyecto/informe/Presentaci%C3%B3n.pdf).

# Note
This project is inspired by [this wonderful project](https://www.electrosmash.com/pedalshield-uno).
