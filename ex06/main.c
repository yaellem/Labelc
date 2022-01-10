#include <avr/io.h>
#include <inttypes.h>

int main()
{
	DDRB = 1 << PB0;
	DDRB = 1 << PB1;
	DDRB = 1 << PB2;
	DDRB = 1 << PB3;
	DDRD = 0;
	PORTB = 0;
	uint8_t c = 0;
	for (;;)
	{
		if  ((PIND & (1 << PD3)) == 0)
		{
			for (uint16_t i = 0; i < 10000; i++);
		//	if (c == 0)
		//		PORTB |= 1 << PB0;
			c = c + 1;
			PORTB = 0;
		}
//		else
//		{
//		}
//		if (((c >> 3) & 1) == 1)
//			PORTB |= 1 << PB3;
//		if ((c >> 2) & 1)
//			PORTB |= 1 << PB2;
//		if ((c >> 1) & 1)
//			PORTB |= 1 << PB1;
		if (c & 1)
			PORTB |= 1 << PB0;
	//	}
	}
	return 0;
}


