# 16x2 LCD Rpi zero driver

## General info
Loadable Kernel Module for driving 16x2 character LCD (1602A). Can be used to write data and commands to LCD.

## Warning
Make sure to connect LCD R/W pin low not to destroy RaspberryPi when connecting LCD directly.

## Compiling
To compile project go to project directory and type 'make'.

## Loading
To load module type command
```sudo insmod LCD_16x2.ko```
This will initiate default pin numbers:
rs	GPIO 17
en	GPIO 27
d0	GPIO 22
d1	GPIO 23
d2	GPIO 24
d3	GPIO 10
d4	GPIO 9
d5	GPIO 25
d6	GPIO 11
d7	GPIO 8

You can pass parameters if your LCD is wired different way.
```sudo insmod LCD_16x2.ko rs_pin=rs en_pin=en d_pin=d0,d1,d2,d3,d4,d5,d6,d7```
where rs, en and d0-d7 are GPIO pin numbers.

## Creating file for character device
To create file for device first you need to know driver major number. It is assigned dynamically when loading module. To find major number use command
```cat /proc/devices```
and search for LCD_16x2. Number on left is its major number.

Now create file for character device with command
```sudo mknod -m 666 /dev/LCD_16x2 c <major_number> 0```

## Writing to device
The driver is meant to be used with C++ library but since it hasn't been created yet you are forced to use ```echo```command.

To write command to lcd you need to write string where first character is \x00 and is followed by all commands that you want to write. For example to initialize display use
```echo -e -n "\x00\x06\x0e\x38" > /dev/LCD_16x2```
This will set cursor in increment mode, set display and cursor on and set 8-bit interface

Writing data is similar but first byte in message is \x01 and can be followed by regular characters not written in hex.
```echo -e -n "\x01Hello world!" > /dev/LCD_16x2```
This will write ```Hello world!``` message to display.

## Sources
* [BCM2835 ARM Peripherials](https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)
* [16x2 LCD commands and timings](https://www.allaboutcircuits.com/technical-articles/how-to-a-162-lcd-module-with-an-mcu/)
