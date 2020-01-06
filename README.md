# MicroDrop-Portable-Biosensor
Created by Brendon Besler, Savitri Butterworth, Thomas Lijnse, Ken Loughery and Katrin Smith, Department of Electrical Engineering, University of Calgary.

Repository for software for ENEL 500 MicroDrop Capstone project - a multi-analyte portable biosensor.

Repository owner: Savitri Butterworth sbutterw@ucalgary.ca

Contents:

\Embedded\
The SAMD21 Atmel project to be programmed to the SAMD21 controllers on the device.
All other files and folders must overwrite the Atmel project defaults when the project is created. To ensure appropriate merging, the Atmel project should be created using the following settings:
1. ADC Active on 
2. 
3. 
4. 
5. 

\Raspberry Pi\
MICRODROP_START.txt is a script to run the GUI, which takes user input and controls runs the tests.

\Raspberry Pi\MicroDrop\
Contains all the files stored on the Raspberry Pi to run the User Interface.

SAM_SPI_Communication.c and .h enable communication over SPI between the Raspberry Pi (master) and Atmel SAM controller (slaves). 
** To use this code spi 0.x and 1.x should both be on and active, performed by modifying the raspberry pi startup configuration file to include activation of the SPI port. https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md
Compilation instructions are provided.

MicroDropGUI.py is the main script to run the GUI. All other files in this directory are dependencies of this script.

