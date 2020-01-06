#include <atmel_start.h>
#include <hal_spi_s_sync.h>

#define START_DATA 0x42
#define START_CYCLE 0x45
#define CONVERSION_TIME 10 //value in micros
#define CONVERSION_COMPLETE 0xaa

#include <hpl_adc1_config.h>
#include <hpl_adc_config.h>
//hpl_adc_config.h
//hpl_adc.c

uint8_t sensor_run;

//arbitrarily chosen:
#define Start_Offset_1			   	1472 //Starting voltage (as a function of MAX DAC VALUE)
#define Pulse_Width_1				100  // plateau length in ms NOTE: takes around 1.5 ms to take samples, need to subtract this (closer to 1 for big numbers)
#define Step_Time_1				200 //MUST BE GREATER THAN PULSE_WIDTH+3
#define Step_Height_1				6 // step offset, will increase by this (as a function of MAX DAC VALUE)
#define Pulse_Amplitude_1			38  //pulse amplitude, will go up and down by this (as a function of MAX DAC VALUE)
#define End_Value_1			        2751 //Ending voltage (as a function of MAX DAC VALUE)

#define MAX_DAC_Voltage_Number		4095 // this is the value of 1 Volt, divide by this to get meaning from step_height and pulse_amplitude

//same as for 1, this is for DAC 2
#define Start_Offset_2			    	1472
#define Pulse_Width_2				100//100   //MUST BE GREATER THAN 8
#define Step_Time_2				200 //500  //MUST BE GREATER THAN PULSE_WIDTH+8
#define Step_Height_2 				6
#define Pulse_Amplitude_2			38//3 Irrelevant for NPV functions...
#define End_Value_2			        2751






void Run_DAC_with_Time(uint16_t Voltage_Num, uint16_t time_delay, uint8_t DAC_Num){
	uint16_t Voltage = Voltage_Num;
	dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
	dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
	//half a delay
	delay_ms(time_delay/2);
	//continue writing to the DAC
	dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
	dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
	//other half of the delay
	delay_ms(time_delay/2);
	//write to the DAC
	dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
	//If the delay was an odd number, will rectify this with an additional 1 ms delay here:
	delay_ms(time_delay%2);
	//write to the DAC just before 1st read:
	dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
}

uint16_t Run_ADC(uint16_t Voltage_Num, uint8_t DAC_Num){
	uint8_t buffer[2];
	uint16_t Voltage = Voltage_Num;
	double result = 0;
	uint16_t temp = 0;
	int16_t temp1 = 0;
	for(uint8_t i = 0; i < 10; i++){
		
		dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
		dac_sync_write(&DAC_0, DAC_Num, &Voltage, 1);
		if(DAC_Num == 1){
			adc_sync_enable_channel(&ADC_1, 0);
			adc_sync_read_channel(&ADC_1, 1, buffer, 2); //changed (&ADC_0, 0, buffer, 2) to (&ADC_0, 1, buffer, 2)
		}
		else
		{
			adc_sync_enable_channel(&ADC_0, 0);
			adc_sync_read_channel(&ADC_0, 0, buffer, 2); //changed (&ADC_0, 0, buffer, 2) to (&ADC_0, 1, buffer, 2)
		}
		//temp = buffer[0]|buffer[1]<<8;
		temp1 = buffer[0]|buffer[1]<<8;
		result += temp1;
	}
	result = result/10;
	temp = (uint16_t)result;
	return temp;
}


int main(void)
{
	
	
	atmel_start_init();
	
	uint16_t total_steps = 0; // total number of steps (both up and down)
	uint16_t DAC_Pulse_Width_1	= Pulse_Width_1 -2; //initialized value (received from SPI)
	uint16_t DAC_Step_Time_1 = Step_Time_1 - Pulse_Width_1 -2;
	uint16_t DAC_Step_Height_1	= Step_Height_1; //initialized value (received from SPI)
	uint16_t DAC_Pulse_Amplitude_1 = Pulse_Amplitude_1; //initialized value (received from SPI)
	uint16_t DAC_Start_Offset_1= Start_Offset_1; //initialized value (received from SPI)
	uint16_t DAC_End_Value_1 = End_Value_1; //initialized value (received from SPI)
	
	uint16_t DAC_Pulse_Width_2	= Pulse_Width_2 - 11; //initialized value (received from SPI)
	uint16_t DAC_Step_Time_2 = Step_Time_2 - Pulse_Width_2 - 11;
	uint16_t DAC_Step_Height_2 	= Step_Height_2; //initialized value (received from SPI)
	uint16_t DAC_Pulse_Amplitude_2 = Pulse_Amplitude_2; //initialized value (received from SPI)
	uint16_t DAC_Start_Offset_2= Start_Offset_2; //initialized value (received from SPI)
	uint16_t DAC_End_Value_2 = End_Value_2; //initialized value (received from SPI)
	double step_number = 0;
	uint16_t highcount = 0;
	//uint16_t lowcount = 0;
	uint8_t buffer[2]; //stores the ADC result, to be passed into the value of the pointer
	uint16_t to_buffer;
	uint16_t stepheight = DAC_Start_Offset_2+DAC_Pulse_Amplitude_2; //value that drives the DAC, will initialize the DAC high
	uint8_t high_or_low = 0;
	
	
	delay_ms(2000);
	
	gpio_set_pin_direction(PA11, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(PA11, GPIO_PULL_DOWN);
	

	
	/* Initializes MCU, drivers and middleware */
	

	spi_s_sync_set_mode(&SPI_0, 0);
	/* Replace with your application code */
	struct io_descriptor *io;
	spi_s_sync_get_io_descriptor(&SPI_0, &io);

	spi_s_sync_enable(&SPI_0);
	
	uint8_t volatile data_in[100];
	uint8_t volatile data_out[2000];
	
	START:data_in[0] = 0x00;
	//io_write(io, data_in, 3);
	
	bool test_flag = true;
	gpio_set_pin_level(PA11, true);
	delay_us(1);
	gpio_set_pin_level(PA11, false);
	delay_us(1);
	gpio_set_pin_level(PA11, true);
	delay_us(1);
	gpio_set_pin_level(PA11, false);
	while(test_flag) {
		io_read(io, data_in, 1);
		if(data_in[0] == 171) {
			//gpio_set_pin_level(PA11, true);
			
			data_out[0] = 0x59;
			//gpio_set_pin_level(PA11, false);
			HANDSHAKE: io_write(io, data_out, 1);
			//for(int i = 0; i < 10; i++) {data_in[i] = 0; data_in[i] = 0;}
			test_flag = false;
			
		}
		if(test_flag == false) {break;}
	}
	
	// monitor for start command
	// This should be interrupt based, but with the board we have we cant :/
	int count = 0;
	while(1){
		data_in[0] = 0;
		io_read(io, data_in, 2);
		if(data_in[0] == START_CYCLE) {sensor_run = data_in[1]; break;}
		else if(data_in[0] == 171) {goto HANDSHAKE;}
		else {count++; if(count > 5) {goto START;}}
	}
	
	// Initialize output array by running conversions
	//dac_sync_enable_channel(&DAC_0, 0);
	//for (uint16_t i = 0; i < 4000; i+=5)
	//{
	//dac_sync_write(&DAC_0,0,&i,1);
	//delay_us(1000);
	//}
	
	if(sensor_run == 2) {

		//calculate the total number of steps in the waveform (not multiplying by 2 yet, just getting the decimal)
		step_number = ((DAC_End_Value_2 - DAC_Start_Offset_2)/ DAC_Step_Height_2);
		//if not even division, this will truncate the total into a whole number:
		total_steps = (uint16_t)step_number;
		if((double)total_steps < step_number){
			//we want to round up if a truncation has occurred, and multiply by two to account for the down steps:
			total_steps = (total_steps+1)*2;
		}
		else{ //if division occurred evenly:
			total_steps = total_steps*2;
		}
		//generate a vector of results, used to store the final information of the appropriate length
		uint8_t result_2_high[total_steps];
		uint8_t result_2_low[total_steps];
		//total_steps = total_steps *2;
		highcount = 0;
		//lowcount = 0;
		stepheight = DAC_Start_Offset_2; //value that drives the DAC, will initialize the DAC high
		

		high_or_low = 0; //0 for a negative pulse, 1 for a positive pulse, negative must come first

		adc_sync_init(&ADC_1, ADC, (void *)NULL); //initialize the values for the second ADC 1
		
		dac_sync_enable_channel(&DAC_0, 1); //enable the DAC 1 channel
		adc_sync_enable_channel(&ADC_1, 0); //0x43000c00 //enable the channel of the ADC
		//$FUNCTION: starting run 2, with {total_steps} steps
		dac_sync_write(&DAC_0, 1, &stepheight, 1); //write the first value, which will be a high offset
		for(uint16_t i = 0; i < total_steps; i++) {
			
			//increment DAC value:
			if(i ==0)
			{
				//initialization phase of base level, no readings taken at this level:
				Run_DAC_with_Time(stepheight,DAC_Step_Time_2,1);
				//increment to first data point:
				stepheight = DAC_Start_Offset_2+DAC_Pulse_Amplitude_2+DAC_Step_Height_2;
				Run_DAC_with_Time(stepheight,DAC_Pulse_Width_2,1);
				// read the value in the ADC
				to_buffer= Run_ADC(stepheight, 1);
				//buffer[0] = to_buffer;
				//buffer[1] = to_buffer >>8;
				//result_2_high[highcount] = ADC_to_mV(buffer);
				result_2_high[highcount] = to_buffer;
				highcount++;
				result_2_high[highcount] = to_buffer >> 8;
				highcount++;
				//write to DAC once more, before shift
				dac_sync_write(&DAC_0, 1, &stepheight, 1);
			}
			else{
				//check to see whether pulse needs to go up or down
				if(high_or_low){
					stepheight += DAC_Step_Height_2 + DAC_Pulse_Amplitude_2;
					high_or_low = 0;
					
					//safety check function, to prevent excessively high voltages
					if(stepheight > MAX_DAC_Voltage_Number)
					{
						stepheight = MAX_DAC_Voltage_Number;
					}

					Run_DAC_with_Time(stepheight,DAC_Pulse_Width_2,1);
					// read the value in the ADC
					to_buffer= Run_ADC(stepheight, 1);
					//buffer[0] = to_buffer;
					//buffer[1] = to_buffer >>8;
					//result_2_high[highcount] = ADC_to_mV(buffer);
					//highcount++;
					result_2_high[highcount] = to_buffer;
					highcount++;
					result_2_high[highcount] = to_buffer >> 8;
					highcount++;
					
					//write to DAC once more, before shift
					dac_sync_write(&DAC_0, 1, &stepheight, 1);
					
				}/////////////////////////////////////////////////////////////////////// THIS CHANGES FOR DPV TO NPV (Stepheight goes to DAC_Start_Offset_x)
				else{
					stepheight -= DAC_Pulse_Amplitude_2;
					high_or_low = 1;
					//safety check function, to prevent excessively low voltages
					if(stepheight < 0)
					{
						stepheight = 0;
					}
					Run_DAC_with_Time(stepheight, DAC_Pulse_Width_1,1);
					//Run_DAC_with_Time(DAC_Start_Offset_2,DAC_Step_Time_2,1);
					// read the value in the ADC
					to_buffer= Run_ADC(stepheight, 1);
					//buffer[0] = to_buffer;
					//buffer[1] = to_buffer >>8;
					//result_2_low[highcount-1] = ADC_to_mV(buffer);
					//lowcount++;
					result_2_low[highcount-2] = to_buffer;
					result_2_low[highcount-1] = to_buffer >> 8;
					
					//write to DAC once more, before shift
					dac_sync_write(&DAC_0, 1, &stepheight, 1);
					
					
				}
			}


		}
		
		//reset the DAC 
		dac_sync_write(&DAC_0, 1, &DAC_Start_Offset_2, 1);
		//disable ADC
		adc_sync_disable_channel(&ADC_1, 0);
		//disable DAC
		dac_sync_disable_channel(&DAC_0, 1);
		
		int j = 2;
		data_out[0] = 0xAA;
		data_out[1] = 0xAA;
		for(int i = 0; i < highcount; i+=2) {
			data_out[j] = result_2_high[i];
			data_out[j+1] = result_2_high[i+1];
			j += 2;
			data_out[j] = result_2_low[i];
			data_out[j+1] = result_2_low[i+1];
			j += 2;
		}
		
		gpio_set_pin_level(PA11, true);
		delay_us(1);
		gpio_set_pin_level(PA11, false);
		delay_us(1);
		} else if (sensor_run == 1) {
		
		
		
		//calculate the total number of steps in the waveform for the second set of data
		step_number = ((DAC_End_Value_1 - DAC_Start_Offset_1)/ DAC_Step_Height_1);
		//if not even division, this will truncate the total into a whole number:
		total_steps = (uint16_t)step_number;
		if(total_steps < step_number){
			//we want to round up if a truncation has occurred, and multiply by two to account for the down steps:
			total_steps = 2*(total_steps+1);
		}
		else{ //if division occurred evenly:
			total_steps = 2*total_steps;
		}
		//generate a vector of results, used to store the final information of the appropriate length
		uint8_t result_1_high[total_steps];
		uint8_t result_1_low[total_steps];
		highcount = 0;
		//lowcount = 0;
		
		
		
		
		stepheight = DAC_Start_Offset_1; //value that drives the DAC, will initialize the DAC high
		high_or_low = 0; //0 for a negative pulse, 1 for a positive pulse, DAC is initially driven high, so negative pulse comes first

		adc_sync_init(&ADC_0, ADC, (void *)NULL); //start the ADC, initializing the 0 values
		dac_sync_enable_channel(&DAC_0, 0); //enable channel 0 of the DAC

		adc_sync_enable_channel(&ADC_0, 0); //enable the 0 channel of the ADC


		dac_sync_write(&DAC_0, 0, &stepheight, 1); //write the first value, which will be a high offset
		
		
		
		
		
		
		for(uint16_t i = 0; i < total_steps; i++) {
			
			//increment DAC value:
			if(i ==0)
			{
				//initialization phase of base level, no readings taken at this level:
				Run_DAC_with_Time(stepheight,DAC_Step_Time_1,0);
				//increment to first data point:
				stepheight = DAC_Start_Offset_1+DAC_Pulse_Amplitude_1+DAC_Step_Height_1;
				Run_DAC_with_Time(stepheight,DAC_Pulse_Width_1,0);
				// read the value in the ADC
				to_buffer= Run_ADC(stepheight, 0);
				//buffer[0] = to_buffer;
				//buffer[1] = to_buffer >>8;
				//result_1_high[highcount] = ADC_to_mV(buffer);
				//highcount++;
				
				result_1_high[highcount] = to_buffer;
				highcount++;
				result_1_high[highcount] = to_buffer >> 8;
				highcount++;
				//write to DAC once more, before shift
				dac_sync_write(&DAC_0, 0, &stepheight, 1);
			}
			else{
				//check to see whether pulse needs to go up or down
				if(high_or_low){
					stepheight += DAC_Step_Height_1 + DAC_Pulse_Amplitude_1;
					high_or_low = 0;
					
					//safety check function, to prevent excessively high voltages
					if(stepheight > MAX_DAC_Voltage_Number)
					{
						stepheight = MAX_DAC_Voltage_Number;
					}

					Run_DAC_with_Time(stepheight,DAC_Pulse_Width_1,0);
					// read the value in the ADC
					to_buffer= Run_ADC(stepheight, 0);
					//buffer[0] = to_buffer;
					//buffer[1] = to_buffer >>8;
					//result_1_high[highcount] = ADC_to_mV(buffer);
					//highcount++;
					result_1_high[highcount] = to_buffer;
					highcount++;
					result_1_high[highcount] = to_buffer >> 8;
					highcount++;
					//write to DAC once more, before shift
					dac_sync_write(&DAC_0, 0, &stepheight, 1);
					
				}
				else{ ///////////////////////////////////////////////////////////// THIS CHANGES FOR DPV TO NPV (Stepheight goes to DAC_Start_Offset_x)
					stepheight -= DAC_Pulse_Amplitude_1;
					high_or_low = 1;
					//safety check function, to prevent excessively low voltages
					if(stepheight < 0)
					{
						stepheight = 0;
					}
					//Run_DAC_with_Time(DAC_Start_Offset_1, DAC_Step_Time_1,0);
					Run_DAC_with_Time(stepheight,DAC_Pulse_Width_1,0);
					// read the value in the ADC
					to_buffer= Run_ADC(stepheight, 0);
					//buffer[0] = to_buffer;
					//buffer[1] = to_buffer >>8;
					//result_1_low[highcount-1] = ADC_to_mV(buffer);
					result_1_low[highcount-2] = to_buffer;
					result_1_low[highcount-1] = to_buffer >> 8;
					//lowcount++;
					//write to DAC once more, before shift
					dac_sync_write(&DAC_0, 0, &stepheight, 1);
					
				}
			}
		}
		
		//reset the DAC
		dac_sync_write(&DAC_0, 0, &DAC_Start_Offset_1, 1);
		//disable ADC
		adc_sync_disable_channel(&ADC_0, 0);
		//disable DAC
		dac_sync_disable_channel(&DAC_0, 0);
		
		
		//code to send result_1 to Raspberry pi
		
		//code to send result_2 to Raspberry pi
		int j = 2;
		data_out[0] = 0xAA;
		data_out[1] = 0xAA;
		for(int i = 0; i < highcount; i+=2) {
			data_out[j] = result_1_high[i];
			data_out[j+1] = result_1_high[i+1];
			j += 2;
			data_out[j] = result_1_low[i];
			data_out[j+1] = result_1_low[i+1];
			j += 2;
		}
	}

	//gpio_set_pin_level(PA11, true);
	//delay_us(1);
	//gpio_set_pin_level(PA11, false);
	//delay_us(1);
	//gpio_set_pin_level(PA11, true);
	//delay_us(1);
	//gpio_set_pin_level(PA11, false);
	//delay_us(1);
	//gpio_set_pin_level(PA11, true);
	//delay_us(1);
	//gpio_set_pin_level(PA11, false);
	//delay_us(1);
	//gpio_set_pin_level(PA11, true);
	//delay_us(1);
	//gpio_set_pin_level(PA11, false);
	//
	//delay_us(1);
	
	uint8_t data_[10];
	data_[0] = CONVERSION_COMPLETE;
	io_write(io, data_, 1);
	
	//delay_us(100);
	// Retrieve configs
	data_in[0] = 0; data_in[1] = 0;
	while(1){
		delay_us(1);
		io_read(io, data_in, 2);
		gpio_set_pin_level(PA11, true);
		delay_us(1);
		gpio_set_pin_level(PA11, false);
		delay_us(1);
		//gpio_set_pin_level(PA11, true);
		if((data_in[0] == START_DATA) && (data_in[1] != 0x00)) {
			int sz_data = data_in[1]*10 - 2;
			for(int i = 0; i < sz_data + 2; i++) {
				io_write(io, &data_out[i], 1);
				delay_us(5);
			}
			goto START;
			//break;
			} else if ((data_in[1] == START_DATA) && (data_in[0] != 0x00)) {
			int sz_data = data_in[0]*10 - 2;
			for(int i = 0; i < sz_data + 2; i++) {
				io_write(io, &data_out[i], 1);
				delay_us(5);
			}
			goto START;
		}
		if((data_in[0] == data_in[1]) && data_in[0] == 171) {goto HANDSHAKE;}
	}
	goto START;
}

//spi_s_sync_transfer(struct spi_s_sync_descriptor *spi, const struct spi_xfer *xfer)

