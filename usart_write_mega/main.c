/*
 * file: main.c
 * author: mr-augustine
 * date: 20160502
 *
 * Exercises the uwrite library for the Mega by printing several test values
 */
#include <stdio.h>
#include "uwrite.h"

//TODO: Excercise the uwrite library on the other serial ports using UCSRnA
// where n == 1, 2, and 3
int main(void) {
    uwrite_init();

    char message[10];

    // We need the carriage return when we use the `screen` command to display 
    // the output on the console.
    snprintf(message, 10, "Mega!\r\n");
    uwrite_print_buff(message);

    uwrite_println_byte("3");

    int a_number = 65535;
    uwrite_println_short(&a_number);

    long a_long_number = 525600;
    uwrite_println_long(&a_long_number);

    return 0;
}
