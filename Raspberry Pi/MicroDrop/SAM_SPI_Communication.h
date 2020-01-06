/******************************************************************************
Written by Thomas Lijnse
ENEL500 uDrop Capstone

This code communicates between the Raspberry PI 0 W and 4 ATSAML21E17B Micros
over the SPI0 port.

Complie with: g++ SPI_Test.cpp -lwiringPi -o SPI_Test

Header file for SPI_Test.cpp
******************************************************************************/

#include <iostream>
#include <string.h>
#include <ostream>
#include <errno.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>

#ifndef SPI_COMMS_H
#define SPI_COMMS_H

using namespace std;

// Use CHANNEL for SPI Channel Select
static const int CHANNEL = 0;

uint8_t volatile buffer_out[1000];
uint8_t volatile buffer_in[1000];
uint8_t volatile buffer[1000];

bool interactive = false;
bool conv_flag = true;
uint8_t err;
uint8_t err_count = 0;


int dat_c;
bool begg_flag;
	   
//char *filename;

clock_t t_start, t_end;

uint8_t num_sens;

time_t rawtime;
struct tm * timeinfo;
   
/////////////////////
// PIN DEFINITIONS //
/////////////////////
#define MIC1 4
#define MIC2 3
#define MIC3 2
#define MIC4 0

/////////////////////
// SPI DEFINITIONS //
/////////////////////
#define SPI_FREQ 500000
#define ENABLE_MIC1_COM digitalWrite(MIC1,0); delayMicroseconds(10)
#define ENABLE_MIC2_COM digitalWrite(MIC2,0); delayMicroseconds(10)
#define ENABLE_MIC3_COM digitalWrite(MIC3,0); delayMicroseconds(10)
#define ENABLE_MIC4_COM digitalWrite(MIC4,0); delayMicroseconds(10)
#define DISABLE_MIC1_COM digitalWrite(MIC1,1);
#define DISABLE_MIC2_COM digitalWrite(MIC2,1);
#define DISABLE_MIC3_COM digitalWrite(MIC3,1);
#define DISABLE_MIC4_COM digitalWrite(MIC4,1);

#define DECODE_TIME 1/SPI_FREQ * 8000000
#define CONV_TIME 50000
#define CONVERSION_TIME_S 90

#define SPI_READ(buffer, size, offset) for(int i = 0; i < (size) + (offset); i++) {buffer[i] = 0;} wiringPiSPIDataRW(CHANNEL, (unsigned char *)(buffer), (size)+(offset)); delayMicroseconds(DECODE_TIME)
#define SPI_WRITE(buffer, size) wiringPiSPIDataRW(CHANNEL, (unsigned char *)(buffer), (size)); delayMicroseconds(DECODE_TIME)

//////////////////////
// TEST DEFINITIONS //
//////////////////////
#define TEST_VAL 0xab
#define TEST_RETURN 0x59
#define CHAR_TEST 0
#define NUM_SAMPLES 213

/////////////////////
// SAM DEFINITIONS //
/////////////////////
#define START_DATA 0x42
#define DATA_STARTED 0x52
#define START_CYCLE 0x45
#define CYCLE_STARTED 0X55
#define CONVERSION_TIME 10 //value in micros
#define CONVERSION_COMPLETE 0xaa
#define ASK_CONV_DONE 0x57

#define SZ_DATA 99 //for each 1 in sz_data, PI will request ten vals from SAM //this will not be static. Can be calculated from the values for the DPV test.
int gah;

int16_t data_retrieved[SZ_DATA*10/2];
uint8_t temp_buff[SZ_DATA*10];

//////////////////////////
// FUNCTION DEFINITIONS //
//////////////////////////
void SPI_Init(void);
uint8_t SPI_Handshake (uint8_t);
void indiv_Char_Test(void);
uint8_t inline checkTimeout(int);
void inline arg_check(int, char**);
int interactive_mode(void);
int auto_mode(uint8_t* , uint8_t);



////////////////////////
// CSV IMPLEMENTATION //
////////////////////////

#ifndef CREATE_CSV_H
#define CREATE_CSV_H



struct dpv_parameters{
	float init_v, final_v, step_h, pulse_w, step_l, pulse_a;
	
	// Basic constructor to initialize variable
	dpv_parameters(float init, float final_v, float step_h, float pulse, float step_l, float pulse_a):
		init_v(init),
		final_v(final_v),
		step_h(step_h),
		pulse_w(pulse),
		step_l(step_l),
		pulse_a(pulse_a){}
};



   dpv_parameters parameters(-0.5, 0.5, 0.005, 100,200, 30);
   int num_columns = 3;
   int column_shift = num_columns +1;
   int test = 1;
	
	string column_header = "Input Voltage [mV], Output Voltage 1 [mV], Output Voltage 2 [mV] \n";
	string filename = "DPV_test1.csv";
	
   int voltage_in[SZ_DATA*10/2], voltage_out_low[SZ_DATA*10/2], voltage_out_high[SZ_DATA*10/2];
   
   

/***
	Takes voltametry values and converts them into a string for the CSV header
	parms: dpv_parameters struct with initial and final voltages, voltage step, pulse width and step length
	shift: number of columns you want to shift the header to the rigth by
	

****/
string create_header(dpv_parameters parms, int shift, int test){
	string shift_columns = "";
	for(int i=0; i<shift; i++){
		shift_columns += " ,";
	}
	string initial_voltage = " intial voltage " +to_string(parms.init_v);
	string final_voltage = " final voltage " + to_string(parms.final_v);
	string step_height = " step heigh " + to_string(parms.step_h);
	string pulse_width = " pulse width "+ to_string(parms.pulse_w);
	string step_length = " step length "+ to_string(parms.step_l);
	string pulse_amplitude = "pulse amplitude " + to_string(parms.pulse_a);
	string parameters = shift_columns +"Parameters used for Test " + to_string(test) + " are:" + initial_voltage 
						+ final_voltage +step_height + pulse_width + step_length+pulse_amplitude+"\n";
	return parameters;
}

string header = create_header(parameters, column_shift, test);
	
/******
	Takes a bunch of inputs to create the csv file
********/
int create_csv_file(string filename, string header, string column_header, const int* const input_v, const int* const output1, const int* const output2){
	time ( &rawtime );
   timeinfo = localtime ( &rawtime );
   FILE *out_file;
   out_file = fopen(filename.c_str(), "w+");
  
   fprintf(out_file, " , , , ,Data collected on %s \n", asctime (timeinfo));
   fprintf(out_file, header.c_str());
   fprintf(out_file, column_header.c_str());
   
	for(int i = 0; i < NUM_SAMPLES; i++) {
	   //cout << "data written is "<< data_retrieved[i] << endl;
	   fprintf(out_file, "%i,", input_v[i]);
		fprintf(out_file,"%i,",output1[i]);
		fprintf(out_file,"%i\n", output2[i]);
	}
  
   fclose(out_file);
}

#endif


/////////////////////////////
// FUNCTION IMPLEMENTATION //
/////////////////////////////

uint8_t inline checkTimeout(int i) {
	t_end = clock();
	if ((((double) (t_end - t_start)) / CLOCKS_PER_SEC) > i) {err = 1; return err;}
}
void indiv_Char_Test (void) {
   	uint8_t temp;
	if(CHAR_TEST) {
		while(1){
			scanf("%d",&temp);
			buffer[0] = temp;
			buffer[1] = 0;
			SPI_WRITE(buffer, 1);
			delay(5);
			SPI_READ(buffer, 1, 1);
			cout << "Buffer contents are: " << buffer[0] << "and " << buffer[1] << endl;
		}
	}	
}
uint8_t SPI_Handshake (uint8_t mic) {
	uint16_t count = 0;
    bool handshake_flag = true;
	uint8_t errflag = 0;
	switch (mic)
	{
		case 1:
		ENABLE_MIC1_COM;
		break;
		case 2:
		ENABLE_MIC2_COM;
		break;
		case 3:
		ENABLE_MIC3_COM;
		break;
		case 4:
		ENABLE_MIC4_COM;
		break;
		default:
		cout << "don't be dumb, this won't work" << endl;
		break;
	}
	
	//SPI_READ(buffer, 10, 0);
    while(handshake_flag) {
		buffer[0] = TEST_VAL;
		SPI_WRITE(buffer,1);
		delayMicroseconds(1000);
		SPI_READ(buffer, 1, 1);
		if(buffer[1] == TEST_RETURN) {
	 	   cout << "Test succesful for microcontroller: "<< (char)(48+mic) << endl;
		   handshake_flag = 0;
	    } else {
		   cout << "Test failed for microcontroller: 1" << " With return value of 0x" << std::hex << buffer_in[0] << endl;
		   buffer[0] = TEST_VAL;
		   count++;
		   if(count == 2000) {cout << "Could not communicate with microcontroller, please try again" << endl; errflag = 1; return errflag;}
	    }
    }
	switch (mic)
	{
		case 1:
		DISABLE_MIC1_COM;
		break;
		case 2:
		DISABLE_MIC2_COM;
		break;
		case 3:
		DISABLE_MIC3_COM;
		break;
		case 4:
		DISABLE_MIC4_COM;
		break;
		default:
		cout << "don't be dumb, this won't work" << endl;
		break;
	}
    cout << "Test Succesful" << endl;
	return errflag;
}

void SPI_Init (void) {
	
// Start Setup //
   wiringPiSetup();
// Declare PINModes //

   pinMode(MIC1, OUTPUT);
   pinMode(MIC2, OUTPUT);
   pinMode(MIC3, OUTPUT);
   pinMode(MIC4, OUTPUT);


// Declare All Channels Inactive // 

   DISABLE_MIC1_COM;
   DISABLE_MIC2_COM;
   DISABLE_MIC3_COM;
   DISABLE_MIC4_COM;


// Fix the fucked up chip selects

   pinMode(23, OUTPUT);
   pullUpDnControl(23, PUD_DOWN);
   digitalWrite(23, 0);
   
   
   pinMode(24, OUTPUT);
   pullUpDnControl(24, PUD_DOWN);
   digitalWrite(24, 0);
   
   
   pinMode(28, OUTPUT);
   pullUpDnControl(28, PUD_DOWN);
   digitalWrite(28, 0);
   
   
   pinMode(29, OUTPUT);
   pullUpDnControl(29, PUD_DOWN);
   digitalWrite(29, 0);
// End Setup //

}

void inline arg_check(int argc, char **argv) {
	if(argv[1][0] == 'h') {
		cout << "Command line arguments for SAM/SPI Communications are as follows" << endl;
		cout << "Enter 'C' as first argument to enter single character send mode" << endl;
		cout << "Enter 'i' as first argument for interactive mode" << endl;
		cout << "Enter numbers, such as ./a.out 1 3 4 as arguments to specify which sensor positions to run conversions on" << endl;
	}
	if((argc == 1) || (argv[1][0] == 'i')) {interactive = true;}
}

int interactive_mode(void) {
	BEGIN: uint8_t mic = 1;
   
   cout << "Enter microcontroller to communicate with: (default is 1)" << endl;
   scanf("%d", &mic);
   
   if (mic > 4 || mic < 1) {cout << mic << "Is not a valid microcontroller " << endl; goto BEGIN;}
   
   
	
   // Start SPI Handshake
   
   
   err = SPI_Handshake(mic);
   if(err) {err = 0; goto RESET;}
   delayMicroseconds(30);
   
   // Send start conversion command
   	switch (mic)
	{
		case 1:
		ENABLE_MIC1_COM;
		break;
		case 2:
		ENABLE_MIC2_COM;
		break;
		case 3:
		ENABLE_MIC3_COM;
		break;
		case 4:
		ENABLE_MIC4_COM;
		break;
		default:
		cout << "Invalid MIC" << endl;
		break;
	}
   delayMicroseconds(10);
   buffer[0] = START_CYCLE;
   SPI_WRITE(buffer, 1);
   //delayMicroseconds(CONVERSION_TIME);
   
   cout << "Start Conversion for sensor " << endl;
   delayMicroseconds(10);
   t_start = clock();
   while (conv_flag) {
	    SPI_READ(buffer, 1, 1);
		//cout << buffer[0] << endl;
		if (buffer[1] == CONVERSION_COMPLETE ) { cout << "Conversion complete" << endl; conv_flag = false;}
		//err = checkTimeout(2 * CONVERSION_TIME_S);
		if(err) {cout << "conversion timed out" << endl; err = 0; goto RESET;}
   }
   // Start read from SAM to test accuracy
   delayMicroseconds(500);
   buffer[0] = START_DATA; buffer[1] = SZ_DATA; //Transmit start data and size of chunk
   SPI_WRITE(buffer, 2);
   cout << "Retrieving Data" << endl;
   delayMicroseconds(4000);
   

   SPI_READ(buffer, 0, 1);
   for(int i= 0; i < SZ_DATA; i++){
	   SPI_READ(buffer, 1, 0);
	   temp_buff[i] = buffer[0];
	   delayMicroseconds(50);
	}
   //SPI_READ(buffer, SZ_DATA*10, 2);
   
   cout << "Creating integers" << endl;
   // Implement some storage thing here
   
   for(int i = 0; i < SZ_DATA; i += 2) {
	   
	   data_retrieved[i/2] = ((temp_buff[i] << 8) + (temp_buff[i+1]));
	   //cout << temp_buff[i] << "and" << data_retrieved[i/2] << "from" << ((temp_buff[i+1] << 8) + (temp_buff[i])) << endl;
   }
   
   cout << "writing to file" << endl;
   	switch (mic)
	{
		case 1:
		DISABLE_MIC1_COM;
		break;
		case 2:
		DISABLE_MIC2_COM;
		break;
		case 3:
		DISABLE_MIC3_COM;
		break;
		case 4:
		DISABLE_MIC4_COM;
		break;
		default:
		cout << "Invalid MIC" << endl;
		break;
	}
   
   time ( &rawtime );
   timeinfo = localtime ( &rawtime );
   FILE *out_file;
   //cout << "Please enter an output filename or file to overwrite" << endl;
   //cin >> filename;
   //filename = "SAM_Indiv_Test.txt"
   out_file = fopen("SAM_INDIV_TEST.txt", "a+");
   fprintf(out_file, "Data collected at %s, \n", asctime (timeinfo));
   for(int i = 0; i < SZ_DATA/2; i++) {
	   //cout << "data written is "<< data_retrieved[i] << endl;
	   fprintf(out_file, "%i \n", data_retrieved[i], '\n');
   }
   fclose(out_file);
   if ((data_retrieved[1] == data_retrieved[SZ_DATA/4]) && (data_retrieved[1] == data_retrieved[SZ_DATA/3]) && (data_retrieved[1] == data_retrieved[SZ_DATA/2])) {
	   cout << "Data likely incorrect, please verify" << endl;
   } else {
   cout << "Data succesfully Written" << endl;
   }
   
   cout << "Quit to Open Test File? (Y/N)" << endl;
   uint8_t yes_no;
   scanf(" %c", &yes_no);
   if(yes_no == 'Y') {
	   return 0;
   }
   goto RESET;
   
   RESET: for(int i = 0; i < SZ_DATA; i++) {
	   buffer[i] = 0x00;
	   temp_buff[i] = 0x00;
	   data_retrieved[i/2] = 0x00;
   } goto BEGIN;
}

int auto_mode(uint8_t *sensor_select, uint8_t num_sensors) {
	for(int i = 0; i < SZ_DATA*10; i++) {
		   buffer[i] = 0x00;
		   temp_buff[i] = 0x00;
		   data_retrieved[i/2] = 0x00;
	   } 
	cout << "Running " << num_sensors << " cycles. Please wait" << endl;
	for(int cycles = 0; cycles < num_sensors; cycles++){
		BEGIN: uint8_t mic = (sensor_select[cycles]-1)/2 + 1;
		// Start SPI Handshake
   
		err = SPI_Handshake(mic);
		if(err) {err_count++; goto RESET;}
		delayMicroseconds(30);
		
		
		switch (mic)
		{
			case 1:
			ENABLE_MIC1_COM;
			break;
			case 2:
			ENABLE_MIC2_COM;
			break;
			case 3:
			ENABLE_MIC3_COM;
			break;
			case 4:
			ENABLE_MIC4_COM;
			break;
			default:
			cout << "Invalid MIC" << endl;
			break;
		}
		
		uint8_t sensor_to_run;
		if(sensor_select[cycles]%2) {
			sensor_to_run = 1;
		} else {
			sensor_to_run = 2;
		}
	   delayMicroseconds(10);
	   buffer[0] = START_CYCLE;
	   buffer[1] = sensor_to_run;
	   SPI_WRITE(buffer, 2);
	   //delayMicroseconds(CONVERSION_TIME);
	   
	   cout << "Start Conversion for sensor " << (int)sensor_select[cycles] << endl;
	   delay(CONV_TIME);
	   //t_start = clock();
	   while (conv_flag) {
			SPI_READ(buffer, 1, 1);
			//cout << buffer[0] << endl;
			if (buffer[1] == CONVERSION_COMPLETE ) { cout << "Conversion complete" << endl; conv_flag = false;}
			//err = checkTimeout(2 * CONVERSION_TIME_S);
			if(err) {cout << "conversion timed out" << endl; err_count++; goto RESET;}
	   }
	   // Start read from SAM to test accuracy
	   delayMicroseconds(500);
	   buffer[0] = START_DATA; buffer[1] = SZ_DATA; //Transmit start data and size of chunk
	   SPI_WRITE(buffer, 2);
	   cout << "Retrieving Data" << endl;
	   delayMicroseconds(4000);
	   

	   SPI_READ(buffer, 0, 1);
	   for(int i= 0; i < SZ_DATA*10-2; i++){
		   SPI_READ(buffer, 1, 0);
		   temp_buff[i] = buffer[0];
		   delayMicroseconds(100);
		}
	   //SPI_READ(buffer, SZ_DATA*10, 2);
	   
	   cout << "Creating integers" << endl;
	   dat_c = 0;
	   begg_flag = true;
	   for(int i = 0; i < SZ_DATA*10/2-1; i++) {
		   while(begg_flag) {
			   if((temp_buff[dat_c] == 0xAA)  && (temp_buff[dat_c+1] == 0xAA)) {
				   dat_c += 2;
				   begg_flag = false;
				   temp_buff[dat_c] = 0xFF;
				   break;
			   } else {
					dat_c++;
			   }
		   }
		   //cout << temp_buff[i];
		   
		   if(((temp_buff[dat_c+1] & 0xF0) == 0xF0) || ((temp_buff[dat_c+1] & 0xF0) == 0x00)) {
				data_retrieved[i] = ((temp_buff[dat_c+1] << 8) + (temp_buff[dat_c]));
				dat_c += 2;
		   } else {
			   dat_c++;
			   data_retrieved[i] = ((temp_buff[dat_c+1] << 8) + (temp_buff[dat_c]));
			   dat_c += 2;
			   
		   }
		   //cout << temp_buff[i] << "and" << data_retrieved[i/2] << "from" << ((temp_buff[i+1] << 8) + (temp_buff[i])) << endl;
	   }
	   
	   cout << "writing to file" << endl;
		switch (mic)
		{
			case 1:
			DISABLE_MIC1_COM;
			break;
			case 2:
			DISABLE_MIC2_COM;
			break;
			case 3:
			DISABLE_MIC3_COM;
			break;
			case 4:
			DISABLE_MIC4_COM;
			break;
			default:
			cout << "Invalid MIC" << endl;
			break;
		}
		
	   gah = 0;
	   for(int i = 0; i < NUM_SAMPLES; i++) {
			voltage_in[i] = (-0.5 + 0.005*i)*1000;
		   voltage_out_low[i] = data_retrieved[gah];
		   voltage_out_high[i] = data_retrieved[gah+1];
		   gah += 2;
	   }
	   
	   
	   switch ((int)sensor_select[cycles]) {
		   case 1:
		   filename = "DPV_test1.csv";
		   break;
		   case 2:
		   filename = "DPV_test2.csv";
		   break;
		   case 3:
		   filename = "DPV_test3.csv";
		   break;
		   case 4:
		   filename = "DPV_test4.csv";
		   break;
		   case 5:
		   filename = "DPV_test5.csv";
		   break;
		   case 6:
		   filename = "DPV_test6.csv";
		   break;
		   case 7:
		   filename = "DPV_test7.csv";
		   break;
		   case 8:
		   filename = "DPV_test8.csv";
		   break;
		   default:
		   filename = "SAM_Tests_Misc";
		   break;
	   }
	   
	   create_csv_file(filename, header, column_header, voltage_in, voltage_out_low, voltage_out_high);
  
	   
	   if ((data_retrieved[1] == 0x00) && (data_retrieved[1] == data_retrieved[SZ_DATA/4]) && (data_retrieved[1] == data_retrieved[SZ_DATA/3]) && (data_retrieved[1] == data_retrieved[SZ_DATA/2])) {
		   cout << "Data likely incorrect, please verify" << endl;
	   } else {
	   cout << "Data succesfully Written" << endl;
	   }
	   
	   
	   //goto RESET;
	   
	   RESET: for(int i = 0; i < SZ_DATA*10; i++) {
		   buffer[i] = 0x00;
		   temp_buff[i] = 0x00;
		   data_retrieved[i/2] = 0x00;
	   } 
	   if(err_count == 5) {return 1;}
	   if(err) {err = 0; goto BEGIN;}
	
	if(cycles+1 < num_sensors) {
		if((sensor_select[cycles]%2) && (sensor_select[cycles+1] - sensor_select[cycles] == 1)) {delay(10000);}
	}
	}
	
}
#endif
