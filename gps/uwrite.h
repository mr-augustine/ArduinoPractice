#ifndef _UWRITE_H_
#define _UWRITE_H_

#define BUFF_SIZE 16

void uwrite_init(void);
void uwrite_print_buff(char * char_buff);
void uwrite_print_byte(void * a_byte);
void uwrite_print_short(void * a_short);
void uwrite_print_long(void * a_long);

#endif

