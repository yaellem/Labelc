#include <avr/io.h>

#define FOSC F_CPU // Clock Speed
#define UART_BAUDRATE 9600
#define READY 1
#define START 2
#define RESET 3
#define O_WINNED 4
#define O_READY 5
#define LOOSER 6
#define WINNER 7
#define PRESSED 8

#include <math.h>

#define UART_PRESCALER ((int)round((float)FOSC / (float)UART_BAUDRATE / 16.0) - 1)

volatile uint8_t ready = 0;
volatile uint8_t master = 0;
volatile uint8_t slave = 0;
volatile uint8_t began = 0;
volatile uint8_t isstarted = 0;
volatile uint8_t win = 0;

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

void	reset()
{
	ready = 0;
	began = 0;
	isstarted = 0;
	slave = 0;
	master = 0;
	win = 0;
}

void	winner()
{
	PORTB |= (1 << PB3) | (1 << PB2) | (1 << PB1) | (1 << PB0);
	for (uint32_t i = 0; i < 200000; i++);
	PORTB = 0;
}

__attribute__ ((signal)) void INT1_vect(void)
{
	while (!(PIND & (1 << PD3)));
	for (uint32_t i = 0; i < 1000; i++);
	
	
	if (ready == 0 && isstarted == 0)
	{
		uart_tx(READY);
		if (slave == 0)
			master = 1;
		ready = 1;
	}
	else if (ready == 1)
	{
		if (began == 1 && isstarted == 1 && slave == 0)
		{
			win = 1;
			uart_tx(LOOSER);
		}
		
		else if (began == 1 && isstarted == 0 && slave == 0)
		{
			win = 0;
			uart_tx(WINNER);
			reset;

		}
		else if (slave == 1)
			uart_tx(PRESSED);
	}	
}

__attribute__ ((signal)) void USART_RX_vect(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	char c;
       	c = UDR0;
	if (c == READY)
	{
		if (ready == 1)
		{
			uart_tx(START);
			began = 1;
			print_rebours();
		}
		else
			slave = 1;
		
	}
	else if (c == START)
	{
		began = 1;
		print_rebours();
	}
	else if (c == PRESSED)
	{
		if (master == 1)
		{
			if (win == 1 || isstarted == 0)
			{
				winner();
				uart_tx(LOOSER);
				reset();
			}
			else
				uart_tx(WINNER);
		}
	}
	else if (c == WINNER)
	{
		reset();
		winner();
	}
	else if (c == RESET || c == LOOSER)
	{
		reset();
	}

}

void USART_Init( unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr & 0xff;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 7data bits, 2stop bit 
	 7 O 2 = 1 startbit, 7 data bits, odd parity bit, 2 stop bit
	 */
	UCSR0C = (1<<UCSZ01); // 7 bits charcter size
	UCSR0C |= (1 << UPM01) | (1 << UPM00); // Odd parity
	UCSR0C |= (1 << USBS0);
	/*Disable transmit flag*/
//	UCSR0A &= ~(1 << TXC0);
	UCSR0A |= (1 << TXC0);
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

	USART_Init(6);

	uart_tx(RESET);
	
	PORTB = 0;
	for (;;)
	{
	}
	return 0;
}
