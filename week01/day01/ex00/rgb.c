#include <avr/io.h>
#define R 0b00101000
#define G 0b01001000
#define B 0b01100000
#define Y 0b00001000
#define C 0b01000000
#define M 0b00100000
#define W 0b00000000

uint8_t color_arr[8] = {
	R, G, B, Y, C, M, W, 0};

int main()
{
	DDRB &= ~(1 << PD2);
	DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3);
	PORTD = 0b11111111;
	uint8_t c = 0;
	for (;;)
	{
		if (!(PIND & (1 << PD2)))
		{
			if (c == 7)
				c=0;
			PORTD = color_arr[c++];
			for (uint32_t i = 0; i < 800000; i++);
		}
	}
	return 0;
}
