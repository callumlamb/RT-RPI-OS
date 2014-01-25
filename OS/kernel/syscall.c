#include <stdint.h>
#include <print.h>
#include <syscall.h>
#include <arch/cpu.h>
#include <arch/keyboard.h>

//TODO implement a basic semaphore instead of disabling and enabling interrupts
//Not much point until my scheduler has a wait queue for sleep support
//Check ptr+len on read doesn't go into kernel space

//Prototypes
int syscall_read(int function, int file, char* ptr, int len);
int syscall_write(int function, int file, char* ptr, int len);

//uint32_t KeyboardCount();

/*
 * syscall_table
 */

//Definition
typedef struct {
	int (*syscall_write)(int function, int file, char* ptr, int len);
	int (*syscall_read)(int function, int file, char *ptr, int len);
} syscall_table_t;

//Create a table, and fill with function pointers
syscall_table_t __attribute__((used)) syscall_table = { syscall_write,
		syscall_read };

/*
 * syscall_write
 *
 * Write to a file, this could be stdout, stderr, so for those redirect to the print to screen methods
 */
int syscall_write(int function, int file, char* string, int len) {
	(void) (function);
	(void) (len);
	switch (file) {
	case 1: //stdout
	case 2:	//stderr
		//Stop interrupts
		CPU_INTERRUPTS_DISABLE();
		int ret = print(string);
		CPU_INTERRUPTS_ENABLE();
		return (ret);
	default:
		return (-1);
	}
}

/*
 * syscall_read
 *
 * Read from a file
 */
int syscall_read(int function, int file, char* ptr, int len) {
	(void) (function);
	if (file == 0) { //stdin
		char* cursor = ptr;
		CPU_INTERRUPTS_DISABLE();
		for (int i = 0; i < len; i++) {
			*cursor++ = keyboard_getc();
		}
		CPU_INTERRUPTS_ENABLE();
		*cursor = '\0';
		return (0);
	} else {
		return (-1);
	}
}

