#include <avr/io.h>

void INT1_vect(void)
{
	PORTB ^= 1 << PB3;
}

int main()
{
//	SREG |= 1 << 7;
	EIMSK = 1 << INT1;

/**		Rising edge **/
//	EICRA = (1 << ISC11) | (1 << ISC10);

/**		Falling edge**/
	EICRA = (1 << ISC11);

/**		Low level**/
//	EICRA = 0;

/**		Any change **/
//	EICRA = (1 << ISC10);

	PORTB = 0;
	DDRB = 1 << PB3;
	DDRD = 0 << PD3;

	for (;;)
	{
		if (EIFR & (1 << INTF1))
		{
//			SREG &= ~(1 << 7);
			for (uint32_t i = 0; i < 500000; i++);
			SREG |= 1 << 7;
		}
	}
	return 0;
}
