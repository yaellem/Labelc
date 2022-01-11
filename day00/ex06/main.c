#include <avr/io.h>

int main()
{
	DDRB = 1 << PB0;
	DDRB |= 1 << PB1;
	DDRB |= 1 << PB2;
	DDRB |= 1 << PB3;
	DDRD = 0;
	PORTB = 0;
	uint8_t c = 0;
	for (;;)
	{
		if  ((PIND & (1 << PD3)) == 0)
		{
			c = c + 1;
			PORTB = c & 0xf;	
			for (uint32_t i = 0; i < 1000000; i++);
		}
	}
	return 0;
}


