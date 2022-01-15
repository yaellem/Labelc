#include <avr/io.h>
#include <util/delay.h>

#define FOSC F_CPU // Clock Speed
#define UART_BAUDRATE 115200
#define READY 1
#define ISSTARTED 2

#include <math.h>

#define UART_PRESCALER ((int)round((float)FOSC / (float)UART_BAUDRATE / 16.0) - 1)

uint8_t ready = 0;
uint8_t isstarted = 0;
uint8_t o_ready = 0;
uint8_t o_winned = 0;

void uart_tx(char c)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

char uart_rx()
{
	while (!(UCSR0A & (1<<RXC0)));
	char c = UDR0;
	return c;
}

void		print_rebours()
{
	PORTB = (1 << PB3);
	for (uint32_t i = 0; i < 200000; i++);
	PORTB = (1 << PB2);
	for (uint32_t i = 0; i < 200000; i++);
	PORTB = (1 << PB1);
	for (uint32_t i = 0; i < 200000; i++);
	PORTB = (1 << PB0);
	for (uint32_t i = 0; i < 200000; i++);
	PORTB = 0;
	isstarted = 1;

}

void	winner()
{
	PORTB |= (1 << PB3) | (1 << PB2) | (1 << PB1) | (1 << PB0);
	for (uint32_t i = 0; i < 200000; i++);
	PORTB = 0;
}

__attribute__ ((signal)) void INT1_vect(void)
{
//	for (uint32_t i = 0; i < 10000; i++);
	if (o_winned == 1)
		o_winned = 0;
	//master || slave
	else if (ready == 0)
	{
		ready = 1;
		uart_tx('r');

	}
	// appuie bouton if le game started
	else if (ready == 1 && o_ready == 1)
	{
		if (isstarted == 1)
		{
			uart_tx('l');
			winner();
			o_winned = 0;
		}
		else if (isstarted == 0)
		{
			uart_tx('w');
			o_winned = 1;
		}
		ready = 0;
		isstarted = 0;
		o_ready = 0;
		PORTB = 0;
	}
	while (PIND & (1 << PD3));
}

__attribute__ ((signal)) void USART_RX_vect(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	char c = UDR0;
	//master || slave
	if (c == 'r')
	{
		o_ready = 1;
		// master
		if (ready == 1)
		{
			isstarted = 0;
			uart_tx('s');
			print_rebours();
		}
	}
	// slave
	else if (c == 's')
	{
		isstarted = 0;
		print_rebours();
	}
	else if (c == 'd' || c == 'l' || c == 'w')
	{
		o_winned = 0;
		ready = 0;
		isstarted = 0;
		PORTB = 0;
		o_ready = 0;
		if (c == 'l')
			o_winned = 1;
		if (c == 'w')
			winner();
	}

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
	/*Disable transmit flag*/
	UCSR0A &= ~(1 << TXC0);
	/* ENABLE INTERRUPT ON RX */
	UCSR0B |= (1 << RXCIE0);
}

int main( void )
{
	//Enable all interrupt
	SREG |= 1 << 7;
	//Enable button interrupt + falling edge
	EIMSK = 1 << INT1;
	EICRA |= (1 << ISC11);
	// Ouptput data register for the button, Input data register pour les led
	DDRD &=  ~(1 << PD3);
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);

	USART_Init(8);
	uart_tx('d');
	PORTB = 0;
	for (;;)
	{
	}
	return 0;
}
