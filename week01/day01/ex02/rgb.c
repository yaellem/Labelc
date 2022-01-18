#include <avr/io.h>

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

uint16_t ft_strlen(uint8_t *str)
{
	uint16_t len = 0;

	while (str[len])
		len++;
	return len;
}

uint8_t char_in_str(uint8_t c, uint8_t *str)
{
	uint16_t i = 0;

	while (str[i])
	{
		if (str[i] == c)
			return 0;
		i++;
	}
	return 1;
}

uint16_t hex_atoi(uint8_t c)
{
	uint16_t pos = 0;
	uint8_t *str = "0123456789abcdef";

	while (str[pos])
	{
		if (str[pos] == c)
			break;
		pos++;
	}
	return pos;
}

uint8_t read_command(void)
{
        uint8_t buf[512];
        uint32_t i = 0;
        char c;
	
        while ((c = uart_rx()) != 13 && i < 512)
        {

                if (c == 127 && i > 0)
                {
                        ft_putstr("\b \b");
                        i--;
                }
                else if (c != 127 && c != 27 && c >= 32)
                {
			if (c >= 'A' && c <= 'Z')
				c += 0x20;
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
	if (buf[0] != '#' || ft_strlen(buf) > 7)
		return 1;
	i = 1;
	while (buf[i])
	{
		if (char_in_str(buf[i++], "0123456789abcdef"))
			return 1;
	}
	
	DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3);
	OCR0A = (hex_atoi(buf[1]) << 4) | (hex_atoi(buf[2]));
	OCR0B = (hex_atoi(buf[3]) << 4) | (hex_atoi(buf[4]));
	OCR2B = (hex_atoi(buf[6]) << 4) | (hex_atoi(buf[6]));
	return 0;
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
	USART_Init(8);
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
	OCR0A = 0x0;	
//	//COMPARE MATCH AT 18 PD5
	OCR0B = 0x0;	
	
	
	// TIMER 2 PD3
	// PD3 SET AT COMPARE MATCH, CLEAR AT BOTTOM
	TCCR2A = (1 << COM2B1) | (1 << COM2B0);

	//TIMER 2 in Fast PWM WITH TOP AT 0xFF WGM22 = 0 WGM21 = 1 WGM20 = 1
	TCCR2A |= (1 << WGM21) | (1 << WGM20);

//	//COMPARE MATCH AT 4f PD3
	OCR2B = 0x0;	

	//NO PRESCALING CS00 = 1
	TCCR0B = (1 << CS00);

	//NO PRESCALING CS20 = 1
	TCCR2B = (1 << CS00);
	uint8_t c = 0;


	for (;;)
	{
		ft_putstr("> ");
		if (read_command())
			ft_putstr("Bad format. Expected: #XXXXXX. (X is a hexa digit)\n\r");
	}
	return 0;
}
