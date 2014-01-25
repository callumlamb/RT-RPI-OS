#include <stdint.h>
#include <arch/framebuffer.h>
#include <print.h>
#include <arch/cpu.h>

static void _print_update();
static void _print_scroll();
static void _print_draw_character(uint8_t character, uint_fast32_t start_x,
		uint_fast32_t start_y);

//Font properties
#define FONT_WIDTH   	 	8
#define FONT_HEIGHT 		16
#define BACK_COLOUR			0
#define FORE_COLOUR			0x7c0

//Terminal properties
#define TERMINAL_WIDTH  	(FRAMEBUFFER_WIDTH/FONT_WIDTH)
#define TERMINAL_HEIGHT 	(FRAMEBUFFER_HEIGHT/FONT_HEIGHT)

//font.bin binary pointers
extern uint8_t _binary_kernel_data_font_bin_start;

//Terminal buffer
uint8_t terminal_screen[TERMINAL_WIDTH * TERMINAL_HEIGHT];
uint8_t terminal_buffer[TERMINAL_WIDTH * TERMINAL_HEIGHT];

//Locations
uint_fast32_t terminal_start = 0;
uint_fast32_t terminal_stop = 0;

/**
 * print_update
 *
 * Update the screen to be the buffer and render all changes on the screen.
 */
static void _print_update() {
	//Loop through each character from the start of changes to the end
	for (uint_fast32_t i = terminal_start;
			i
					< terminal_stop
							+ (TERMINAL_WIDTH - (terminal_stop % TERMINAL_WIDTH));
			i++) {
		//If the buffer is different from the screen draw the change and update the screen buffer
		if (terminal_screen[i] != terminal_buffer[i]) {
			_print_draw_character(terminal_buffer[i], i % TERMINAL_WIDTH,
					i / TERMINAL_WIDTH);
			terminal_screen[i] = terminal_buffer[i];
		}
	}
	//Set the start of the changes back to the cursor
	terminal_start = terminal_stop;
}

/**
 * print_scroll
 *
 * Scroll the text buffer one row up
 */
static void _print_scroll() {
	uint_fast32_t i = 0;
	//Move all character TERMINAL_WIDTH backwards
	for (; i < (TERMINAL_WIDTH * TERMINAL_HEIGHT) - (TERMINAL_WIDTH); i++)
		terminal_buffer[i] = terminal_buffer[i + TERMINAL_WIDTH];
	for (; i < (TERMINAL_WIDTH * TERMINAL_HEIGHT); i++)
		terminal_buffer[i] = 0;	//Clear last row
	terminal_stop -= TERMINAL_WIDTH;	//Move terminal_stop back	
	terminal_start = 0;	//Do a full screen update
}

/**
 * print_print
 *
 * Print text on the screen
 *
 * @param str The string to print.
 */
int print(char *str) {

	//Keep count
	int count = 0;

	//Update buffer
	while (*str) {
		//Get next character
		uint8_t character = *str++;
		//Handle new lines
		if (character == '\n') {
			//Move end of change to next line		
			terminal_stop += TERMINAL_WIDTH - (terminal_stop % TERMINAL_WIDTH);
		}
		//Handle normal characters
		else {
			//Place character in buffer			
			terminal_buffer[terminal_stop] = character;
			terminal_stop++;
		}
		//Scroll if necessary
		if (terminal_stop >= (TERMINAL_WIDTH * TERMINAL_HEIGHT))
			_print_scroll();

		//Update count
		count++;
	}

	//Update the screen
	_print_update();

	//Return count
	return (count);

}

/**
 * _print_draw_character
 *
 * Draws a character at the specified x,y pixel coordinate
 *
 * @param character Ascii character to draw
 * @param start_x x coordinate pixel to draw at
 * @param start_y y coordinate pixel to draw at
 */
static void _print_draw_character(uint8_t character, uint_fast32_t start_x,
		uint_fast32_t start_y) {
	uint16_t* fb = framebuffer_get();
	if (character < 33 || character > 126)
		character = 0; //Don't bother with non-printable characters
	uint8_t* charAddress = (uint8_t*) &_binary_kernel_data_font_bin_start
			+ (character * FONT_HEIGHT); //get address of character in binary blob
	//loop through each column of the character
	for (uint_fast32_t row = 0; row < FONT_HEIGHT; row++) {
		uint8_t *bits = charAddress + row;//Get address of row of bits in the font file		
		uint_fast16_t y = ((row + (start_y * FONT_HEIGHT)) * FRAMEBUFFER_WIDTH); //Get offset into framebuffer where the first row of bits will go	
		//Loop through each row of the character
		for (uint_fast8_t bit = 0; bit < FONT_WIDTH; bit++) {
			uint_fast16_t x = (bit + (start_x * FONT_WIDTH));//Calculate offset from y into framebuffer this bit will go			
			uint16_t colour = BACK_COLOUR;
			if (*bits >> bit & 0x1)
				colour = FORE_COLOUR;
			*((volatile uint16_t *) fb + x + y) = colour;	//Write the pixel	
		}
	}
}
