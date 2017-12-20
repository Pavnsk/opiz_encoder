
//#include <wiringPi.h>
#include "opiz_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rotaryencoder.h"

int numberofencoders = 0;

void updateEncoders()
{
	struct encoder *encoder = encoders;
	for (; encoder < encoders + numberofencoders; encoder++)
	{
		int MSB = opiz_gpio_input(encoder->pin_a);
		int LSB = opiz_gpio_input(encoder->pin_b);		

		int encoded = (MSB << 1) | LSB;
		int sum = (encoder->lastEncoded << 2) | encoded;

		if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoder->value++;
		if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoder->value--;

		encoder->lastEncoded = encoded;
	}
}

struct encoder *setupencoder(unsigned int pin_a, unsigned int pin_b)
{
	if (numberofencoders > max_encoders)
	{
		printf("Maximum number of encodered exceded: %i\n", max_encoders);
		return NULL;
	}

	struct encoder *newencoder = encoders + numberofencoders++;
	newencoder->pin_a = pin_a;
	newencoder->pin_b = pin_b;
	newencoder->value = 0;
	newencoder->lastEncoded = 0;

	opiz_gpio_set_cfgpin(pin_a, OPIZ_GPIO_INPUT);
	opiz_gpio_set_cfgpin(pin_b, OPIZ_GPIO_INPUT);
	opiz_gpio_pullup(pin_a, OPIZ_PULL_UP);
	opiz_gpio_pullup(pin_b, OPIZ_PULL_UP);
	
	return newencoder;
}