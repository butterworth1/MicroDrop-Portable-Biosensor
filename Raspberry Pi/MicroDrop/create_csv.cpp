/***********************************************
	create_csv.ppt
	Written by Brendon Besler for uDrop capstone project
	March 2019
	
	Test program to write out a csv file for received data on pi
	compile on windows command line with 'g++ create_csv.cpp -std=c++0x -o csv'
**************************************************/

#include <iostream>
#include <string>
#include <ostream>
#include <errno.h>
#include <time.h>
#include <unistd.h>

using namespace std;

#define SZ_DATA 255

time_t rawtime;
struct tm * timeinfo;

struct dpv_parameters{
	float init_v, final_v, step_h, pulse_w, step_l,pulse_v;
	
	// Basic constructor to initialize variable
	dpv_parameters(float init, float final_v, float step_h, float pulse, float step_l):
		init_v(init),
		final_v(final_v),
		step_h(step_h),
		pulse_w(pulse),
		step_l(step_l),
		pulse_v(pulse_v){}
};

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
	string parameters = shift_columns +"Parameters used for Test " + to_string(test) + " are:" + initial_voltage 
						+ final_voltage +step_height + pulse_width + step_length+"\n";
	return parameters;
}

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
   
	for(int i = 0; i < SZ_DATA/2; i++) {
	   //cout << "data written is "<< data_retrieved[i] << endl;
	   fprintf(out_file, "%i,", input_v[i]);
		fprintf(out_file,"%i,",output1[i]);
		fprintf(out_file,"%i\n", output2[i]);
	}
  
   fclose(out_file);
}


int main(){
	cout << "Creating integers" << endl;
   // Implement some storage thing here
   
   // Test parameters in double format
   dpv_parameters parameters(-0.8, 0.4, 0.002, 20,100);
   int num_columns = 3;
   int column_shift = num_columns +1;
   int test = 1;
	string header = create_header(parameters, column_shift, test);
	
	string column_header = "Input Voltage [mV], Output Current 1 [mA], Output Current 2 [mA] \n";
	string filename = "DPV_test1.csv";
	
   int data_retrieved1[SZ_DATA/2], data_retrieved2[SZ_DATA/2], data_retrieved3[SZ_DATA/2];
   
   for(int i = 0; i < SZ_DATA; i += 2) {
	   //data_retrieved[i/2] = ((temp_buff[i] << 8) + (temp_buff[i+1]));
	   //cout << temp_buff[i] << "and" << data_retrieved[i/2] << "from" << ((temp_buff[i+1] << 8) + (temp_buff[i])) << endl;
	   data_retrieved1[i/2] = i;
	   data_retrieved2[i/2] = i * 10 + i;
	   data_retrieved3[i/2] = i * 10 + i*2;
   }
   
   create_csv_file(filename, header, column_header, data_retrieved1, data_retrieved2, data_retrieved3);
   
   if ((data_retrieved1[1] == data_retrieved1[SZ_DATA/4]) && (data_retrieved1[1] == data_retrieved1[SZ_DATA/3]) && (data_retrieved1[1] == data_retrieved1[SZ_DATA/2])) {
	   cout << "Data likely incorrect, please verify" << std::endl;
   } else {
   cout << "Data succesfully Written" << std::endl;
   }
   
}