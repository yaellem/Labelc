#include <avr/io.h>
#include <util/delay.h>

#define FOSC F_CPU // Clock Speed
#define UART_BAUDRATE 115200
#define USERNAME "ymarcill"
#define PASSWD "root"
#define MAXLEN 512

/*__attribute__ ((signal)) void PCINT1_vect(void)
{
	PORTB = 0;
}*/

uint8_t str_cmp(uint8_t *s1, uint8_t *s2)
{
	uint32_t i;
	for (i = 0; s1[i] != '\0'; i++)
	{
		if (s1[i] != s2[i])
			break;
	}
	return (s1[i] - s2[i]);
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

void ft_putstr(uint8_t *str)
{
	uint32_t index = 0;
	
	while (str[index])
		uart_tx(str[index++]);
}

uint8_t	ask_passwd(void)
{
	uint8_t buf[MAXLEN];
	uint32_t i = 0;
	char c;
	
	ft_putstr("\e[1;34m\tpassword: \e[0m");
	while ((c = uart_rx()) != 13 && i < MAXLEN)
	{
		if (c == 127 && i > 0)
		{
			ft_putstr("\b \b");
			i--;
		}
		else if (c != 127 && c!= 27 && c >= 32 )
		{
			buf[i++] = c;
			uart_tx('*');
		}
		else if (c == 27)
		{
			c = uart_rx();
			c = uart_rx();
		}

	}
	buf[i] = '\0';
	ft_putstr("\n\r");
	if (!str_cmp(buf, PASSWD))
	{
		ft_putstr("\e[1;32mWelcome ");
		ft_putstr(USERNAME);
		ft_putstr(" ! My true and only friend\e[0m\n\r");
		PORTB = (1 << PB3);
		_delay_ms(200);
		PORTB = (1 << PB2);
		_delay_ms(200);
		PORTB = 1 << PB1;
		_delay_ms(200);
		PORTB = 1 << PB0;
		_delay_ms(200);
		PORTB = (1 << PB2) | (1 << PB3) | (1 << PB1) | (1 << PB0);
		_delay_ms(200);
		PORTB = 0;
		return (0);
	}
	return (1);
}

uint8_t	ask_username(void)
{
	uint8_t buf[MAXLEN];
	uint32_t i = 0;
	char c;

	PORTB = 0;
	ft_putstr("\e[1;34m\tusername: \e[0m");
	while ((c = uart_rx()) != 13 && i < MAXLEN)
	{

		if (c == 127 && i > 0)
		{
			ft_putstr("\b \b");
			i--;
		}
		else if (c != 127 && c != 27 && c >= 32) 
		{
			buf[i++] = c;
			uart_tx(c);
		}
		else if (c == 27)
		{
			c = uart_rx();
			c = uart_rx();
		}
	}
	buf[i] = '\0';
	ft_putstr("\n\r");
	if (!str_cmp(buf, USERNAME))
	{
		if (!ask_passwd())
			return 0;
	}
	ft_putstr("\e[0;31mMauvaise combinaison username/password my lord (lol try again dumbass)\n\n\n\r\e[0m");
	PORTB = (1 << PB2) | (1 << PB3) | (1 << PB1) | (1 << PB0);
	return 1;

}

int main( void )
{
	DDRB = (1 << PB3);
	DDRB |= (1 << PB2);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB0);
	USART_Init(8);
	for (;;)
	{
		ft_putstr("Bonjour mon ami(e), entrez vos identifiants je vous prie:\n\r");
		if (!ask_username())
			return 0;
		//char c = uart_rx();
		//uart_tx(c);
	}
	return 0;
}
