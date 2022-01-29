#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

void uart_tx(char c)
{
        while (!(UCSR0A & (1<<UDRE0)));
        UDR0 = c;
}

uint16_t adc_read()
{
	ADCSRA |= (1 << ADSC); // If single conversion mode: start a conversion If Free running mode: start first conversion
	while ((ADCSRA & (1 << ADSC)));
	return ADCL | ADCH << 8;
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

int main()
{
	uint8_t str[4];
	USART_Init(8);
	PRR &= ~(1 << PRADC);
	ADMUX = (1 << REFS0) | (1 << REFS1); // Voltage reference : Internal 
//	ADMUX |= (1 << ADLAR); // LEFT ADJUST TO JUST READ ADCH to get the data
	ADMUX |= (1 << MUX3); //ADC8
	ADCSRA = (1 << ADEN); //Enable ADC

	for (;;)
	{
		//row value
		uint16_t new = (((adc_read())));// - (273 + 100 - 0x0002)) * 128) / 0x0003) +25;
		itoa(new, str, 10);
		uart_tx(str[0]);
		uart_tx(str[1]);
		uart_tx(str[2]);
		uart_tx('\n');
		uart_tx('\r');
		_delay_ms(1000);
		//celsius value
		uint16_t cels = (((new - (273 + 100 - 0x0002)) * 128) / 0x0003) +25;
		itoa(cels, str, 10);
		uart_tx(str[0]);
		uart_tx(str[1]);
		uart_tx('\n');
		uart_tx('\r');
		_delay_ms(1000);

	}
	return 0;
}
