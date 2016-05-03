/*
 * file:    ledbutton.h
 * author:  mr-augustine
 * date:    20160502
 * 
 * Declares the functions used for a SPST pushbutton that has an LED indicator.
 * The actual button I used for this was from Radio Shack (# 275-0009).
 *
 * Regardless of the button's initial physical state (pressed or unpressed),
 * this library will initialize the button as being unpressed.
 */
#ifndef _LED_BUTTON_H_
#define _LED_BUTTON_H_

// Sets up the button
uint8_t button_init(void);

// Checks the status of the button and updates its state
void button_update(void);

// Returns 1 if the button is pressed; 0 otherwise
uint8_t button_is_pressed(void);

// Turns the button's LED off
void led_turn_off(void);

// Turns the button's LED on
void led_turn_on(void);

#endif /* _LED_BUTTON_H_ */
