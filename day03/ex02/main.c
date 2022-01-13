#include <avr/io.h>

#define FOSC F_CPU // Clock Speed
#define UART_BAUDRATE 115200

char uart_rx(void)
{
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void uart_tx(char c)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
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

/*__attribute__ ((signal)) void TIMER1_COMPA_vect(void)
{
	char c = uart_rx();
	uart_tx(c);	
}*/

void main( void )
{
	USART_Init(8);
	/** TIMER 1 settings **/
/*	SREG |= 1 << 7;
	TCCR1A = (1 << COM1A0) | (1 << WGM11) | (1 << WGM10); // FAST PWM, TOGGLE
	TCCR1B = (1 << WGM12) | (1 << WGM13); // FAST PWM
	TCCR1B |= (1 << CS10);
	OCR1A = 1600;
	TIMSK1 = 1 << OCIE1A;*/
	for (;;)
	{
		char c = uart_rx();
		uart_tx(c);
	}
}
