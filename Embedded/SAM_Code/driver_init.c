/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

#include <hpl_adc_base.h>

struct adc_sync_descriptor ADC_0 =
{
	.device.channel = 0
	
};
struct adc_sync_descriptor ADC_1 =
{
	.device.hw = 0x43000c00,
	.device.channel = 1
	
};

struct spi_s_sync_descriptor SPI_0;

struct dac_sync_descriptor DAC_0;

void ADC_0_PORT_init(void)
{

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA03, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA03, PINMUX_PA03B_ADC_AIN1);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA07, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA07, PINMUX_PA07B_ADC_AIN7);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA09, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA09, PINMUX_PA09B_ADC_AIN17);
}

void ADC_0_CLOCK_init(void)
{
	hri_mclk_set_APBDMASK_ADC_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, ADC_GCLK_ID, CONF_GCLK_ADC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void ADC_0_init(void)
{
	ADC_0_CLOCK_init();
	ADC_0_PORT_init();
//	adc_sync_init(&ADC_0, ADC, (void *)NULL);
}

void SPI_0_PORT_init(void)
{
	gpio_set_pin_direction(PA11, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(PA11, GPIO_PULL_DOWN);
	
	gpio_set_pin_direction(PA18, GPIO_DIRECTION_OFF);
	gpio_set_pin_pull_mode(PA18, GPIO_PULL_DOWN);
	// Set pin direction to input
	gpio_set_pin_direction(PA16, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(PA16,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA16, PINMUX_PA16D_SERCOM3_PAD0);

	gpio_set_pin_level(PA17,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(PA17, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(PA17, PINMUX_PA17D_SERCOM3_PAD1);

	// Set pin direction to input
	gpio_set_pin_direction(PA24, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(PA24,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA24, PINMUX_PA24C_SERCOM3_PAD2);

	gpio_set_pin_level(PA19,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(PA19, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(PA19, PINMUX_PA19D_SERCOM3_PAD3);
}

void SPI_0_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_SLOW, CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM3_bit(MCLK);
}

void SPI_0_init(void)
{
	SPI_0_CLOCK_init();
	spi_s_sync_init(&SPI_0, SERCOM3);
	SPI_0_PORT_init();
}

void DAC_0_PORT_init(void)
{

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA02, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA02, PINMUX_PA02B_DAC_VOUT0);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA05, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA05, PINMUX_PA05B_DAC_VOUT1);
}

void DAC_0_CLOCK_init(void)
{

	hri_mclk_set_APBCMASK_DAC_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, DAC_GCLK_ID, CONF_GCLK_DAC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void DAC_0_init(void)
{
	DAC_0_CLOCK_init();
	dac_sync_init(&DAC_0, DAC);
	DAC_0_PORT_init();
}

void system_init(void)
{
	init_mcu();

	ADC_0_init();

	SPI_0_init();

	DAC_0_init();
}