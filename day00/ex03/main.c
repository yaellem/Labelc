#include <avr/io.h>

long delay;

int main()
{
	DDRB = 1 << PB3;
	for (;;)
	{
		for (delay = 0; delay < 200000; delay++);
		PORTB ^= 1 << PB3;
	}
	return 0;
}


