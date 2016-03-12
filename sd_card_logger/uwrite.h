/*
 *
 */
#ifndef _uwrite_H_
#define _uwrite_H_

#define RCIE0   7
#define RXEN0   4
#define TXEN0   3
#define UCSZ01  2
#define UCSZ00  1
#define UDRE0   5

#define BUFF_SIZE 16

void uwrite_init(void);
void uwrite_print_buff(const char * char_buff);
void uwrite_print_byte(const void * a_byte);
void uwrite_print_short(const void * a_short);
void uwrite_print_long(const void * a_long);

#endif

