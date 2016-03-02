#include <stdio.h>
#include "uwrite.h"

int main(void) {
    UWRITE_init();

    char message[10];

    snprintf(message, 10, "Hello!\n");
    UWRITE_print_buff(message);

    UWRITE_print_byte("3");

    int a_number = 20;
    UWRITE_print_short(&a_number);

    return 0;
}
