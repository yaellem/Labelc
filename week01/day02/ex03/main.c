#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

void uart_tx(char c)
{
        while (!(UCSR0A & (1<<UDRE0)));
        UDR0 = c;
}

uint8_t adc_read()
{
	ADCSRA |= (1 << ADSC); // If single conversion mode: start a conversion If Free running mode: start first conversion
	while ((ADCSRA & (1 << ADSC)));
	return ADCH;
}

void	adc_init()
{
	PRR &= ~(1 << PRADC);
	ADMUX = (1 << REFS0);// Voltage reference : AVcc
	ADMUX |= (1 << ADLAR); // LEFT ADJUST TO JUST READ ADCH to get the data
	ADMUX |= (1 << MUX2); //ADC48 Temp measurement
	ADCSRA = (1 << ADEN); //Enable ADC
}

void USART_Init( unsigned int ubrr)
{
        /*Set baud rate */
        UBRR0H = (uint8_t)(ubrr >> 8);
        UBRR0L = (uint8_t)ubrr & 0xff;
        /*Enable receiver and transmitter */
        UCSR0B = (1<<RXEN0)|(1<<TXEN0);
        /* Set frame format: 8data bits, 1stop bit 
         8 N 1 = 1 startbit, 8 data its, no parity bit, 1 stop bit
         */
        UCSR0C = (1<<UCSZ00)|(1<<UCSZ01); // 8bits charcter size
        /** CLEAR TRANSMIT FLAG BEFORE TRANSMITTING **/
        UCSR0A &= ~(1 << TXC0);
}

void	timer_init()
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

//      //COMPARE MATCH AT f2 PD6
        OCR0A = 0x4b;
//      //COMPARE MATCH AT 18 PD5
        OCR0B = 0x00;


        // TIMER 2 PD3
        // PD3 SET AT COMPARE MATCH, CLEAR AT BOTTOM
        TCCR2A = (1 << COM2B1) | (1 << COM2B0);

        //TIMER 2 in Fast PWM WITH TOP AT 0xFF WGM22 = 0 WGM21 = 1 WGM20 = 1
        TCCR2A |= (1 << WGM21) | (1 << WGM20);

//      //COMPARE MATCH AT 4f PD3
        OCR2B = 0x82;

        //NO PRESCALING CS00 = 1
        TCCR0B = (1 << CS00);

        //NO PRESCALING CS20 = 1
        TCCR2B = (1 << CS00);

}

void	pin_init()
{
	DDRD &= ~(1 << PD2); // button input
	DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3);
	DDRB |= 1 << PB0| 1 << PB1| 1<< PB2| 1 << PB3;
	PORTB = 0;
}


int main()
{
	uint8_t str[4];
	uint8_t compteur = 0;

	USART_Init(8);
	adc_init();
	timer_init();
	pin_init();
	for (;;)
	{
		uint8_t c = adc_read();
		if (!(PIND & (1 << PD2)))
		{
			compteur++;
			if (compteur == 3)
				compteur = 0;
			_delay_ms(100);
		}
		if (compteur == 0)
			OCR0A = c;
		else if (compteur == 1)
			OCR0B = c;
		else if (compteur == 2)
			OCR2B = c;
		
		if (c >= 62 && c <= 68)
			PORTB =  1 << PB0;
		else if (c >= 126 && c <= 130)
			PORTB =  1 << PB1;
		else if (c >= 189 && c <= 193)
			PORTB =  1 << PB2;
		else if (c == 0xff)
			PORTB =  1 << PB3;
		_delay_ms(100);
	}
	return 0;
}
