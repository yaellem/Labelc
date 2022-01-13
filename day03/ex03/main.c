#include <avr/io.h>

#define FOSC F_CPU // Clock Speed
#define UART_BAUDRATE 115200

void uart_tx(char c)
{
	while (!(UCSR0A & (1<<UDRE0)));

	if (c >= 'A' && c <= 'Z')
		c += 0x20;
	else if (c >= 'a' && c <= 'z')
		c-=0x20;
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
	// ENABLE INTERRUPT ON RX
	UCSR0B |= (1<< RXCIE0);
}

__attribute__ ((signal)) void USART_RX_vect(void)
{
	while (!(UCSR0A & (1<<RXC0)));
	char c = UDR0;
	uart_tx(c);
}

void main( void )
{
	// Enable all interrupt
	SREG |= 1 << 7;
	//ENABLE INT ON RX
	USART_Init(8);

	for (;;);
}
