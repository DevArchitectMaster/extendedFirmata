# [extendedFirmata](https://github.com/DevArchitectMaster/extendedFirmata)
**by [DevArchitectMaster](https://github.com/DevArchitectMaster)**

*--- tested for Firmata 2.5.8 ---*



## Goal and Approach

This project is an extension of the existing [Firmata project [www.github.com/firmata/arduino]](www.github.com/firmata/arduino).

The goal was to be able to address external input (e.g. sensors) and output devices (e.g. lcd display) via Python with the Arduino. For this purpose, a new INO template for the Arduino was developed, which can be extended very easily. For communication, strings are sent asynchronously, which are interpreted by the corresponding callback functions. It is only important that the given communication format and its order, as well as the corresponding control characters are kept. The control character ```###``` has been defined. A complete string to be sent or received looks like the following:
```python
###operation###callnumber###result###
```

* ```operation``` = id of the sensor to be addressed  
=> sensor 1 returns random values to be able to test the setup as well as the own program

* ```callnumber``` = id of the asynchronous call, so that later the correct answer can be assigned to the correct caller

* ```result``` = current measured value of the sensor

**ATTENTION**:
> The whole communication string must not exceed 32 characters due to the hardware & software architecture of the microcontroller!

> If the microcontroller does not get a valid communication string, the complete text is further sent to the output device lcd display.



## Tips and Instructions for Use and own Extension

I recommend to use the two ```*_template``` (```.py``` & ```.ino```)-files and to adapt or complete them as needed. If you want to include more sensors, you can do this in the ```*.ino```-file at the correspondingly commented position. To do this, search for the ```// EDIT: //``` comments in the source code and add the appropriate places:

> ``#include <SensorLibrary.h>``  
=> the new library must be imported here

> ```SensorLibrary sl(pin1);```  
=> in the ```INIT SENSORS```-area the corresponding sensor must be initialized and calibrated, i.e. under which pin the sensor can be found

> ```sensor.begin();```  
=> in the ```setup()```-function the corresponding sensor must be started (mostly with the ```begin()```-method)

> ```case X: # dSensorValue = sensor.read(); # break;```  
=> in the ```getSensorValue()```-function the (read) access to the corresponding sensor must be done

**BEFORE:**  
Before new input or output devices can be added, the appropriate library must be downloaded and installed in the [Arduino IDE](https://www.arduino.cc/en/Main/Software_). To do this you should have the Arduino IDE software running at this time. Next it’s necessary to install the appropriate library, which can be done though the *Arduino Library Manager*: ```Sketch→Include Library→Manage Libraries…```. Enter in the search field what library you are searching for and look through the list for it. Click the ```Install```-button, or ```Update``` from an earlier version.  
=> Now you are ready for programming! :-)



**TIP:**  
*As a tip at the end, try to comment everything new directly, as the existing source code is already commented, so that you can find the right places in the source code at a later time.*



## Supported **input** (*sensors*) and **output** (*screen*) devices

### **input** (*sensors*) devices
* [DHT.md](docs/input/DHT.md)  
  => humidity and temperature sensor
* ...

### **output** (*screen*) devices
* [LiquidCrystal](docs/output/LiquidCrystal.md)  
  => LiquidCrystal displays (LCDs) based on the Hitachi HD44780 (or a compatible) chipset
* ...


## Errata

I would be very happy, if you use my program that you give me short feedback.  
You are welcome to post found bugs as [```git-issues```](https://github.com/DevArchitectMaster/extendedFirmata/issues) and to connect them with a corresponding [```merge-request```](https://github.com/DevArchitectMaster/extendedFirmata/pulls). I also accept further wishes or improvements as [```git-issues```](https://github.com/DevArchitectMaster/extendedFirmata/issues), too.

**Thank you for your trust in me =)**