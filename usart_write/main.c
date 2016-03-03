#include <stdio.h>
#include "uwrite.h"

int main(void) {
    UWRITE_init();

    char message[10];

    // We need the carriage return when we use the `screen` command to display 
    // the output on the console.
    snprintf(message, 10, "Hello!\r\n");
    UWRITE_print_buff(message);

    UWRITE_print_byte("3");

    int a_number = 65535;
    UWRITE_print_short(&a_number);

    long a_long_number = 525600;
    UWRITE_print_long(&a_long_number);

    return 0;
}
