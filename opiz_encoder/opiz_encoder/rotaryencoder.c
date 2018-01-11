
//#include <wiringPi.h>
#include "opiz_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rotaryencoder.h"

int numberofencoders = 0;

double gettimesec()
{
	double value;
	double value2;
	struct timespec cur_time;
	clock_gettime(CLOCK_MONOTONIC, &cur_time);
	value = cur_time.tv_sec;
	value2 = ((double)cur_time.tv_nsec) / 1000000000;
	return value + value2;
}

void updateEncoders()
{
	struct encoder *encoder = encoders;
	double cur_time = gettimesec();

	for (; encoder < encoders + numberofencoders; encoder++)
	{
		if (cur_time - encoder->previous_time > 0.001)
		{
			int MSB = opiz_gpio_input(encoder->pin_a);
			int LSB = opiz_gpio_input(encoder->pin_b);

			int encoded = (MSB << 1) | LSB;
			int sum = (encoder->lastEncoded << 2) | encoded;

			if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoder->value++;
			if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoder->value--;

			encoder->lastEncoded = encoded;
			encoder->previous_time = cur_time;
		}
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
	
	newencoder->previous_time = gettimesec();

	opiz_gpio_set_cfgpin(pin_a, OPIZ_GPIO_INPUT);
	opiz_gpio_set_cfgpin(pin_b, OPIZ_GPIO_INPUT);
	opiz_gpio_pullup(pin_a, OPIZ_PULL_UP);
	opiz_gpio_pullup(pin_b, OPIZ_PULL_UP);
	
	return newencoder;
}