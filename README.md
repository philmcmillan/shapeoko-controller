shapeoko-controller
===================

Arduino Mega2560 based controller for Shapeoko to eliminate need for USB connected laptop/computer.

Reads from SD card and streams to grbl on a UART (Serial1 on the Mega2560).

**Current**

  * Implements UI on 4 row 20 column character LCD
  * Depends upon Arduino hardware abstraction and environment for build
  * No in controller jogging - requires USB UART connection override

** Planned**
  * Build versioning
  * Arduino environment independence using avr-gcc directly
  * Jogging implementation
  * Better pin mapping header strategy
  * Far future: independence from Arduino hardware abstraction libraries

** Setup and Build**

Currently, with dependency on Arduino environment for compilation, all of the classes/subdirectories
need a symbolic link from the Arduino libraries directory for the Arduino compilation to
find, compile and link the project.
