*** The RFDuino is now discontinued - we suggest to use the ESP based version***

## RFDuino


This is the first iteration of the platform which supports the now discontinued RFDuino. 

This project features a wireless logging of force sensitive sensors which could be placed on a shoe's insole.

## Hardware Requirements
- RFDuino + Coincell Battery Sheet
- up to 6 force sensitive resistors (https://www.adafruit.com/products/166)
- 3V battery
- For wiring, please check `./fritzing/`




## Client Installation (Unix and OS X only)
Ardafruit Bluetooth LE Library (get it from https://github.com/adafruit/Adafruit_Python_BluefruitLE) but there is also a copy in the repository


    cd Adafruit_Python_BluefruitLE
    sudo python setup.py install


## Usage

- start the arduino by inserting a 3V battery
- start the server with `python python-client.py` in the terminal