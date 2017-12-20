#define max_encoders 3

struct encoder
{
	unsigned int pin_a;
	unsigned int pin_b;
	volatile long value;
	volatile int lastEncoded;
};

struct encoder encoders[max_encoders];

struct encoder *setupencoder(unsigned int pin_a, unsigned int pin_b); 
