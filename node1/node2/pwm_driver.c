#include "pwm_driver.h"
#include "sam.h"

bool pwm_init(uint32_t clock_frequency){
	PMC->PMC_PCER0 = (1u << ID_PIOB); // Enable clock for PIOB
	PMC->PMC_PCER1 = (1u << (ID_PWM - 32)); // Enable clock for PWM
	 
	PWM->PWM_WPCR = 0x50574D00; // Write Protection Key
	PWM->PWM_WPCR |= (1u << 2); // Disable Write Protection for PWM registers
	PWM->PWM_DIS = 0xFFu; // Disable all PWM channels
	PWM->PWM_IDR1 = 0xFFFFFFFFu; // Disable all PWM interrupts
	PWM->PWM_IDR2 = 0xFFFFFFFFu; // Disable all PWM interrupts

	PWM->PWM_SCM = 0; // Disable synchronous channels
	PWM->PWM_FMR = 0; // Disable fault mode
	
	// Configure PWM settings
	PWM->PWM_CLK = clock_frequency / 1000000u; // Set clock to 1 MHz
	return true;
}

bool pwm_start(uint8_t channel, uint32_t period, uint32_t high_time){
	if(channel > 3){
		return false;
	}

	if (high_time > period || period > 65535) {
		return false;
	}

	uint32_t mask = 1u << (12u + channel); // Calculate the mask for the specified channel
	PIOB->PIO_CODR = mask; // Clear the output for the specified channel
	PIOB->PIO_OER = mask; // Enable output for the specified channel
	PIOB->PIO_PER = mask; // Enable PIO control for the specified channel
	
	PWM->PWM_CH_NUM[channel].PWM_CMR = 11u | (1u << 9);
	PWM->PWM_CH_NUM[channel].PWM_CPRD = period; // Set period
	PWM->PWM_CH_NUM[channel].PWM_CDTY = high_time; // Set high time

	PIOB->PIO_ABSR |= mask; // Set alternate function B for the specified channel
	PIOB->PIO_OER = mask; // Enable output for the specified channel
	PIOB->PIO_PER = mask; // Enable PIO control for the specified channel

	PWM->PWM_ENA = (1 << channel); // Enable channel
	
	return true;
}

bool pwm_write_duty(uint8_t channel, uint32_t high_time){
	if(channel > 3){
		return false;
	}
	
	PWM->PWM_CH_NUM[channel].PWM_CDTY = high_time; // Set high time
	
	return true;
}