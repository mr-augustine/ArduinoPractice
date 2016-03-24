#include <stdio.h>
#include "uwrite.h"

int main(void) {
    uwrite_init();

    char message[10];

    // We need the carriage return when we use the `screen` command to display 
    // the output on the console.
    snprintf(message, 10, "Hello!\r\n");
    uwrite_print_buff(message);

    uwrite_print_byte("3");

    int a_number = 65535;
    uwrite_print_short(&a_number);

    long a_long_number = 525600;
    uwrite_print_long(&a_long_number);

    return 0;
}
