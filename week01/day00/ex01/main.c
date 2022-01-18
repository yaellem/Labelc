#include <avr/eeprom.h>
#include <avr/io.h>
#define MAXLEN 512
#define DUMP_CMD "hexdump"
#define W_CMD "write"

uint8_t uart_rx(void)
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

void	ft_putnbr(uint32_t nbr)
{

	if (nbr > 9)
	{
		ft_putnbr(nbr / 10);
		ft_putnbr(nbr % 10);
	}
	else
		uart_tx(nbr + 48);

}

uint8_t	read_ee(uint16_t addr)
{
	uint8_t c;

	while (EECR & (1 << EEPE)); //Wait for a potential write instr to be finisehd to read
	EEAR = addr & 0x3ff; //Fill EEAR with my address masked to fill the 10bits register
	EECR |= (1 << EERE); //Put EERE bit to one enabling reading operation
	c = EEDR; //8bit EEDR is filled by the previous read operation
	return c;
}

void	write_ee(uint16_t addr, uint8_t c)
{
	while (EECR & (1 << EEPE)); //Wait for a potential write instr to be finisehd to read
	EEAR = addr & 0x3ff; //Fill EEAR with my address masked to fill the 10bits register
	EEDR = c; // Fill the EDDR register with the data we want to write
	EECR |= (1 << EEMPE); // Enabling master write, needed before setting EEPE to write, action that will begin the operation
	EECR |= (1 << EEPE);
}

void	print_dump()
{
	uint32_t addr = 0;
	uint8_t data_byte;
	uint8_t c;
	uint8_t hexh;
	uint8_t hexl;
	uint8_t *hex = "0123456789ABCDEF";
	
	while (addr < 1024)
	{
		if ((addr % 0x10) == 0)
		{
			if (addr > 0)
				ft_putstr("\n\r");
			ft_putnbr(addr);
		}
		uart_tx(' ');
		uart_tx(' ');
		c = read_ee(addr) & 0xff;
//		hexh = hex[c & 0xf0];
//		hexl = hex[c & 0x0f];
		uart_tx(hexh);
		uart_tx(hexl);
		uart_tx(c & 0xff);
		uart_tx(' ');
		addr++;
	}
}

uint8_t	read_command(void)
{
	uint8_t buf[MAXLEN];
	uint32_t i = 0;
	uint8_t c;

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
	if (!str_cmp(buf, DUMP_CMD))
	{
		print_dump();
		ft_putstr("\n\r");
		return 0;
	}
	else if (!str_cmp(buf, W_CMD))
	{
		ft_putstr("  max 512 chars > ");
		i = 0;
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
		i = 0;
		while (buf[i])
		{
			write_ee(i, buf[i] & 0xff);
			i++;
		}
		return 0;
	}
	return 1;
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
	//Button pin is in out state
//	DDRD &= ~(1 << PD3);
	USART_Init(8);
	for (;;)
	{
		ft_putstr("> ");
		if (read_command())
			ft_putstr("Command not found. Only known: write [ENTER] \"my value\", hexdump\n\r");
	}
	return 0;
}
