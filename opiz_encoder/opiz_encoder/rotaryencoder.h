#include <time.h>
#define max_encoders 4

struct encoder
{
	unsigned int pin_a;
	unsigned int pin_b;
	volatile long value;
	volatile int lastEncoded;
	double  previous_time;
};

struct encoder encoders[max_encoders];

struct encoder *setupencoder(unsigned int pin_a, unsigned int pin_b); 
