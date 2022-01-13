#include <avr/io.h>

#define FOSC F_CPU // Clock Speed
#define UART_BAUDRATE 115200
#define MYUBRR FOSC/16/UART_BAUDRATE-1



void uart_tx(char c)
{
	UDR0 = c;
}

void USART_Init( unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data bits, 1stop bit 
	 8 N 1 = 1 startbit, 8 data its, no parity bit, 1 stop bit
	 */
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01); // 8bits charcter size
	/** CLEAR TRANSMIT FLAG BEFORE TRANSMITTING **/
	UCSR0A |= (1 << TXC0);
}

__attribute__ ((signal)) void TIMER1_COMPA_vect(void)
{

	uart_tx('Z');
//	uart_tx('l');
//	uart_tx('o');
//	uart_tx('\n');
//	DDRB |= 1 << PB3;
//	PORTB = 1<< PB3;
}

void main( void )
{
	USART_Init(MYUBRR);
	/** TIMER 1 settings **/
	SREG |= 1 << 7;
	TCCR1A = (1 << COM1A1) | (1 << COM1A0);
	TCCR1B = (1 << WGM12);
	TCCR1B |= (1 << CS12);
//	TCCR1B |= (1 << CS12) | (1 << CS10);
	OCR1A = F_CPU /256;
//	OCR1A = F_CPU /512;
	TIMSK1 = 1 << OCIE1A;
	for (;;);
}
