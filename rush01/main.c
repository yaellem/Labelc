#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

__attribute__((signal)) void TIMER1_COMPA_vect(void)
{
	static uint16_t compteur = 1000;
	static uint16_t save = 0;

	PORTC = (1 << PC1);
	if (compteur == 1000)
		save = 1000;
	else if (compteur <= 3)
	{
		save = 0;
	}
	if (save == 1000)
		OCR1A = compteur--;
	else if (save = 0)
		OCR1A = compteur++;
}

void uart_tx(char c)
{
        while (!(UCSR0A & (1<<UDRE0)));
        UDR0 = c;
}

void	ft_putstr(char *str)
{
	uint32_t i = 0;
	while (str[i])
	{
		uart_tx(str[i]);
		i++;
	}
}

uint16_t adc_read()
{
        ADCSRA |= (1 << ADSC); // If single conversion mode: start a conversion If Free running mode: start first conversion
        while ((ADCSRA & (1 << ADSC)));
       // return ADCH << 8 | ADCL;
        return ADCH;
}

void    adc_init()
{
        PRR &= ~(1 << PRADC);
        ADMUX = (1 << REFS0);// Voltage reference : AVcc
        ADMUX |= (1 << ADLAR); // LEFT ADJUST TO JUST READ ADCH to get the data
        ADMUX |= (1 << MUX2) | (1 << MUX0); //ADC5
        ADCSRA = (1 << ADEN); //Enable ADC
}

void    pin_init()
{
        DDRD &= ~(1 << PD2); // button input
        DDRB |= 1 << PB0| 1 << PB1| 1<< PB2| 1 << PB3;
        DDRC &= ~(1 << PC5);
	DDRC |= (1 << PC1);
	PORTB = 0;
}

void    timer_init()
{
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
        OCR0A = 0x0;
//      //COMPARE MATCH AT 18 PD5
        OCR0B = 0x0;


        // TIMER 2 PD3
        // PD3 SET AT COMPARE MATCH, CLEAR AT BOTTOM
        TCCR2A = (1 << COM2B1) | (1 << COM2B0);

        //TIMER 2 in Fast PWM WITH TOP AT 0xFF WGM22 = 0 WGM21 = 1 WGM20 = 1
        TCCR2A |= (1 << WGM21) | (1 << WGM20);

//      //COMPARE MATCH AT 4f PD3
        OCR2B = 0x0;

        //NO PRESCALING CS00 = 1
        TCCR0B = (1 << CS00);

        //NO PRESCALING CS20 = 1
        TCCR2B = (1 << CS20);
	
	//TIMER 1
        //PC1 SET AT COMPARE MATCH, CLEAR AT BOTTOM
	TCCR1A |= (1 << COM1A1) | (1 << COM1A0);
        TCCR1A |= (1 << WGM11);
	TCCR1B |= (1<< WGM13) | (1 << WGM12);

	//Enable interrupt
	SREG |= 1 << 7;
	TIMSK1 |= (1 << OCIE1A);
	ICR1 = 1000;
	OCR1A = 1000;
//      NO PRESCALER

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

void	debug_tx(uint8_t conversion, uint8_t base, uint32_t max, uint8_t ret)
{
	uint8_t str[6];

	itoa(conversion, str, base);
	uint32_t i = 0;
	while (i < max)
	{
		uart_tx(str[i]);
		i++;
	}
	if (ret == 1)
	{
		uart_tx('\n');
		uart_tx('\r');
	}
}

int main()
{
	uint8_t str[6];
	USART_Init(8);
	timer_init();
	pin_init();
	adc_init();
	uint8_t bas = 0;
	uint8_t haut = 0;
	uint8_t state = 0;
	uint8_t mode = 0;
	ft_putstr("Start\n\r");
	for (;;)
	{
		if (!(PIND & (1 << PD2)))
		{
			if (mode == 3)
				mode = 0;
			mode++;
			if (mode == 3)
				ft_putstr("Rx mode\n\r");
			else if (mode ==2)
				ft_putstr("Tx mode Potentiometre\n\r");
			else if (mode ==1)
				ft_putstr("Tx mode auto\n\r");
			while (!(PIND & (1 << PD2)));
			_delay_ms(100);
		}
		if (mode == 3)
		{
			PORTC = 0;
			DDRC = 0;
			PORTB = 0;
        		DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3);
			TCCR1B &= ~(1 << CS10);
			uint8_t r = adc_read();
			debug_tx(r, 16, 2, 1);

			if (r == 0)
			{
				if (state == 1)
					bas++;
			}
			else
			{
				if (bas != 0)
				{
					//bas -= 10;
					uint8_t total = haut + bas;
					if (total != 16)
					{
						bas = bas - ((haut + bas) -  16);
						total = haut + bas;
					}
				
					debug_tx(haut, 10, 3, 0);
					uart_tx('|');
					debug_tx(bas, 10, 3, 1);
	
					uint8_t pcbas = (haut * 100) / total;
					debug_tx(pcbas, 10, 3, 1);
					if (pcbas <= 17)
					{
						OCR0A = 0xff;
						uint8_t variant =(((pcbas * 100) / 17) * 0xff) / 100; 
						debug_tx(variant, 10, 3, 1);
						OCR0B = variant;
						OCR2B = 0;
					}
					else if (pcbas > 17 && pcbas <= 34)
					{
						OCR0A = (((pcbas * 100) / 40) * 0xff) / 100;
						debug_tx(OCR0A, 10, 3, 1);
						OCR0B = 0xff;
						OCR2B = 0;
					}
					else if (pcbas > 34 && pcbas <= 50)
					{
						OCR0A = 0;
						OCR0B = 0xff;
						OCR2B = (((pcbas * 100) / 60) * 0xff) / 100;
						debug_tx(OCR2B, 10, 3, 1);
					}
					else if (pcbas > 50 && pcbas <= 66)
					{
						OCR0A = 0;
						OCR0B = (((pcbas * 100) / 80) * 0xff) / 100;
						debug_tx(OCR0B, 10, 3, 1);
						OCR2B = 0xff;
					}
					else if (pcbas > 66 && pcbas <= 82)
					{
						OCR0A = (((pcbas * 100) / 100) * 0xff) / 100;
						debug_tx(OCR0A, 10, 3, 1);
						OCR0B = 0;
						OCR2B = 0xff;
					}
					else if (pcbas > 82 && pcbas <= 100)
					{
						OCR0A = 0xff; 
						OCR0B = (((pcbas * 100) / 100) * 0xff) / 100;
						debug_tx(OCR0B, 10, 3, 1);
						OCR2B = 0;
					}
					haut = 0;
					bas = 0;
				}
	
				state = 1;
				haut++;
			}
		}
		else if (mode == 1)
		{
        		DDRD &= ~(1 << PD6) & ~(1 << PD5) & ~(1 << PD3);
			DDRC = (1 << PC1) | (1 << PC5);;
			TCCR1B |= 1 << CS10;
			PORTB = 1 << PB0;
//			if (TCNT1 == 0)
//				PORTC = (1 << PC1);
		}
		else if (mode == 2)
		{
			//USE INPUT CAPTURE UNIT
			PORTB = 1 << PB1;
		}
		
	}
	return 0;
}
