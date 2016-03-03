#ifndef _UWRITE_H_
#define _UWRITE_H_

#define BUFF_SIZE 256

void UWRITE_init(void);
void UWRITE_print_buff(char * character);
void UWRITE_print_byte(void * a_byte);
void UWRITE_print_short(void * a_short);
void UWRITE_print_long(void * a_long);

#endif
