# sensor-insole-prototyping
##Using off-the-shelf sensors for flexible shoe sensing



This project features a wireless logging of force senstive sensors which could be placed on a shoe's insole.

## Installation 
    
    cd Adafruit_Python_BluefruitLE
    sudo python setup.py install

## Requirements

- Ardafruit Bluetooth LE Library (get it from https://github.com/adafruit/Adafruit_Python_BluefruitLE) but there is also a copy in the repository
- RFDuino 
- up to 6 force sensitive resistors (https://www.adafruit.com/products/166)
- 3V battery

## Usage

- start the arduino by inserting a 3V battery
- start the server with `python python-client.py` in the terminal
