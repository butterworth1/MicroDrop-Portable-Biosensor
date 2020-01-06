
int num_steps = (int)((parms.final_v - parms.init_v)/v_step); //assuming the final voltage is a low voltage
double input_v[num_steps];

struct dpv_parameters{
	float init_v, final_v, step_h, pulse_w, step_l, pulse_v;
	
	// Basic constructor to initialize variable
	dpv_parameters(float init, float final_v, float step_h, float pulse, float step_l, float pulse_v):
		init_v(init),
		final_v(final_v),
		step_h(step_h),
		pulse_w(pulse),
		step_l(step_l),
		pulse_v(pulse_v){}
};

input_voltage(double*  ,paramters parms){
	for(int i=0; i<num_steps; i++){
		input_v[i] = parms.init_v + i*v_step;
	}
	return input_v
}