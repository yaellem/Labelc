#include <avr/io.h>

int main()
{
	DDRB = 1 << PB3;
	for (;;)
		PORTB = 1 << PB3;
	return 0;
}
