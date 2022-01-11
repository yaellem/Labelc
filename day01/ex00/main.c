#include <avr/io.h>

int main()
{
	DDRB |= 1 << PB1;
	TCCR1B &= 0xE0;
	TCCR1B |= 1 << WGM12;
//	TCCR1B |= (1 << CS12);
	TCCR1A &= 0x7C;
	TCCR1A |= 1 << COM1A0;
// If the cpu goes to 16 000 000 h/s, 0,5 will be 8 000 000, but this number doesnt fit in an 16-bit register, so we need to reduce it, and reduce proportionnaly the clock time, so to get a plain number we can divide : 8 000 000 / 256 wich give us 31 250, so we divide the clock time by 256 with the prescaler
//	OCR1A = F_CPU / 2 / 256;
	OCR1A = 9000;
	for (;;)
	{

	}
	return 0;
}
