#include <avr/io.h>

uint32_t color_arr[3] = {0x00f2184f, 0x0019e0a1, 0x004b0082};

int main()
{
	DDRB &= ~(1 << PD2);
	//PD3 = OC2B      PD5 = OC0B      PD6 = OC0A
	//Le but etant de faire des duty cycle de BOTTOM a la valeur donnee, sur une frqeuence tres rapide afin de changer l'intensite 
	// TIMER 0 : PD5 PD6, Mode fast pwm pour pouvoir faire BOTTOM -> TOP en 1 periode et set at BOTTOM
	// PD6 SET AT COMPARE MATCH. CLEAR AT BOTTOM
	TCCR0A = (1 << COM0A1) | (1 << COM0A0);
	
	//PD5 SET AT COMPARE MATCH, CLEAR AT BOTTOM
	TCCR0A |= (1 << COM0B1) | (1 << COM0B0);

	//TIMER 0 in FAST PWM WITH TOP AT 0xFF WGM02 = 0 WGM01 = 1 WGM00 = 1
	TCCR0A |= (1 << WGM01) | (1 << WGM00);

//	//COMPARE MATCH AT f2 PD6
//	OCR0A = (color_arr[0] >> 16) & 0xff;
	OCR0A = 0x0;	
//	//COMPARE MATCH AT 18 PD5
//	OCR0B = (color_arr[0] >> 8) & 0xff;
	OCR0B = 0x0;	
	
	
	// TIMER 2 PD3
	// PD3 SET AT COMPARE MATCH, CLEAR AT BOTTOM
	TCCR2A = (1 << COM2B1) | (1 << COM2B0);

	//TIMER 2 in Fast PWM WITH TOP AT 0xFF WGM22 = 0 WGM21 = 1 WGM20 = 1
	TCCR2A |= (1 << WGM21) | (1 << WGM20);

//	//COMPARE MATCH AT 4f PD3
//	OCR2B = color_arr[0] & 0xff;
	OCR2B = 0x0;	

	//NO PRESCALING CS00 = 1
	TCCR0B = (1 << CS00);

	//NO PRESCALING CS20 = 1
	TCCR2B = (1 << CS00);
	uint8_t c = 0;
	for (;;)
	{
		if (!(PIND & (1 << PD2)))
		{
			DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3);
			if (c >= 3)
				c = 0;
			//COMPARE MATCH AT f2 PD6
			OCR0A = (color_arr[c] >> 16) & 0xff;
			//COMPARE MATCH AT 18 PD5
			OCR0B = (color_arr[c] >> 8) & 0xff;
			//COMPARE MATCH AT 4f PD3
			OCR2B = (color_arr[c]) & 0xff;
			c++;
			for (uint32_t i = 0; i < 800000; i++);
		}
	}
	return 0;
}
