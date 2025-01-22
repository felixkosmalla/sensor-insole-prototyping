## ESP8266


This is second iteration of the platform using WiFi and an ESP8266.


## Hardware Requirements
- Wemos D1
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