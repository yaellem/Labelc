#include <avr/io.h>

static volatile uint8_t  c = 0;

__attribute__ ((signal)) void TIMER0_COMPA_vect(void)
{
	static uint8_t i = 0;

	if (c == 0)
		i = 0;
	if (i == 0)
		OCR1A = c++;
	if (c == 255)
		i = 255;
	if (i == 255)
		OCR1A = c--;
				
//	if (OCR1A < ICR1 && c < ICR1)
//	{
//		OCR1A += 1;
//	}
//	else if (OCR1A > 0)
//	{
//		OCR1A -= 1;
//	}
//	if (OCR1A == 0)
//		c = OCR1A;
//	else
//		c++;
	//OCR1A = ICR1 / c;


}

int main()
{
	DDRB |= 1 << PB1;


/** TIMER 1 **/
        TCCR1B &= 0xE0;
        TCCR1B |= (1 << WGM13) | (1 << WGM12);
        TCCR1A &= 0x3C;
        TCCR1A |= (1 << WGM11);
        TCCR1A |= (1 << COM1A1) | (1 << COM1A0);
//        ICR1 = F_CPU / 256;
//        OCR1A = ICR1 / 10;
        ICR1 = 256;
        OCR1A = 0;
//        TCCR1B |= (1 << CS12);
	TCCR1B |= (1 << CS10);

/** TIMER 0 **/

	SREG |= 1 << 7;
	TCCR0A |= (1 << WGM01);
	OCR0A = 255;
	TCCR0B = (1 << CS02);
        
	TIMSK0 |= 1 << OCIE0A;
	for (;;);
	{
		if (TIFR0 & (1 << OCF0A))
		{
			SREG |= 1 << 7;
			TIFR0 |= 1 << OCF0A;
		}
	}
	return 0;
}
