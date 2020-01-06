# MicroDrop-Portable-Biosensor
Created by Brendon Besler, Savitri Butterworth, Thomas Lijnse, Ken Loughery and Katrin Smith, Department of Electrical Engineering, University of Calgary.

Repository for software for ENEL 500 MicroDrop Capstone project

UI Folder contains all relevant .py functions for the Raspberry PI User Interface

PI_Code contains the cpp and header file for the communications over SPI between the raspberry PI and Atmel SAM controller. These files are for the PI, and constitute the SPI Master controller
** To use this code spi 0.x and 1.x should both be on and active, performed by modifying the raspberry pi startup configuration file to include activation of the SPI port. https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md
Compilation instructions are provided

All other files and folders must overwrite the Atmel project defaults when the project is created. To ensure appropriate merging, the Atmel project should be created using the following settings:
1. ADC Active on 
2. 
3. 
4. 
5. 
