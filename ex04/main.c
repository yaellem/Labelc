#include <avr/io.h>

volatile unsigned long delay;

int main()
{
	DDRB = 1 << PB3;
	DDRD = 0;
	for (;;)
	{
		if  (PIND & (1 << PD3))
			PORTB = 0;
		else
			PORTB |= 1 << PB3;
	}
	return 0;
}


