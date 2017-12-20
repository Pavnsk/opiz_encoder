
#include "opiz_gpio.h"
#include "stdio.h"
#include "rotaryencoder.h"

int main()
{
	opiz_gpio_init();
	
	struct encoder *encoder = setupencoder(OPIZ_GPA(0), OPIZ_GPA(1));
	long value;
	while (1)
	{
		updateEncoders();
		long l = encoder->value;
		if (l != value)
		{
			printf("value: %d\n", (void *)l);
			value = l;
		}
	}
	return(0);
}