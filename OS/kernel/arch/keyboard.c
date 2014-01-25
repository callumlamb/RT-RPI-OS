#include <stdint.h>
#include <arch/keyboard.h>
#include <print.h>
#include <arch/clock.h>

//Library functions
uint32_t UsbInitialise();
void UsbCheckForChange();
uint32_t KeyboardCount();
uint32_t KeyBoardGetAddress(uint32_t index);

/*
 * keyboard_init
 *
 * Initialise the keyboard
 */
void keyboard_init() {
	//Initialise usb
	if (UsbInitialise() != 0) {
		print("Error initialising USB driver.\n");
	}
	//Wait for keyboard to appear
	while (KeyboardCount() == 0) {
		print("Keyboard not found, trying again...\n");
		clock_wait(0xF4240);
		UsbCheckForChange();
	}
	print("Keyboard device found.\n");
}

/*
 * keyboard_getc
 *
 * Get a character from the keyboard
 */
char keyboard_getc() {
	return 'a';
}
