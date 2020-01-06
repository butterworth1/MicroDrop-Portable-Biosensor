
/******************************************************************************
Written by Thomas Lijnse
ENEL500 uDrop Capstone

This code communicates between the Raspberry PI 0 W and 4 ATSAML21E17B Micros
over the SPI0 port.

To compile, on the PI use
>  g++ SPI_Test.cpp -lwiringPi -o SPI_Test

Or a similar filename and output executable name

******************************************************************************/
//#include "create_csv.h"
#include "SAM_SPI_Communication.h"


int main(int argc, char *argv[])
{
	cout << argc << " arguments provided are: " << endl;
	for(int i = 1; i < argc; ++i) {cout << argv[i] << endl;}
	
   arg_check(argc, argv);
   if(argv[1] == "help") {return 0;}
   
   SPI_Init();
	
   int fd, result;

   cout << "Initializing" << endl ;

   // Configure the interface.
   fd = wiringPiSPISetup(CHANNEL, SPI_FREQ);

   cout << "Init result: " << fd << endl;
   
   if(argv[1][0] == 'C') {indiv_Char_Test();}
   
   if(interactive) {return interactive_mode();}

	cout << "Please enter an output filename or file to append to" << endl;
	//cin >> filename;
	filename = (char*)"Test_Outputs.txt";
	num_sens = argc-1;
	uint8_t sensor_select[num_sens];
	for(int i = 0; i < num_sens; i++) {sensor_select[i] = (uint8_t)argv[i+1][0]-48;}
	
	cout << "Starting test " << sensor_select[0] << endl;
   return auto_mode(sensor_select, num_sens);
   
}
