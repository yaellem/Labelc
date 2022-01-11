#include <avr/io.h>
#include <inttypes.h>

int main()
{
	DDRB = 1 << PB3;
	DDRD = 0;
	uint8_t state = 0;
	PORTB = 0;
	for (;;)
	{
		if ((PIND & (1 << PD3)) == 0)
		{
			state ^= 1;
			if (state == 1)
				PORTB = 1 << PB3;
			else if (state == 0)
				PORTB = 0;
			while ((PIND & (1 << PD3)) == 0);
		}
		for (uint32_t i = 0; i < 14000; i++);

	}
	return 0;
}


