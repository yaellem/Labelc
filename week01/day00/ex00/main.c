#include <avr/eeprom.h>
#include <avr/io.h>
#define MAG0 0xAA
#define MAG1 0x55

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

int main()
{
	//Button pin is in out state
	DDRD &= ~(1 << PD3);
	//Led pins are in out state
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
	uint8_t c;
	//Check if, magic number is set, if yes, read it and don't write in EEPROM
	if ((c = read_ee(0x0)) == MAG0 && (c = read_ee(0x1)) == MAG1)
		c = read_ee(0x2);
	// Setting magic number MAG0 to 0x0 addr, then MAG1 to 0x1 addr, then Write compteur in EEPROM
	else
	{
		write_ee(0, MAG0);
		write_ee(1, MAG1);
		c = 0;
		write_ee(0x2, c);
	}
	PORTB = 0;
	//Read magic number 
	for (;;)
	{
		if (!(PIND & (1 << PD3)))
		{
			c++;
			write_ee(0x2, c);
			PORTB = c & 0xf;
			for (uint32_t i = 0; i < 800000; i++);
		}
	}
	return 0;
}
