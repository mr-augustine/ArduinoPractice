/*
 * file:    ledbutton.c
 * author:  mr-augustine
 * date:    20160502
 *
 */
#ifdef DEBUG
#include "uwrite.h"
#endif

#include <avr/io.h>
#include "ledbutton.h"
#include "pins.h"

#define NEW_PIN_VALUE   ((BUTTON_PINVEC & (1 << BUTTON_PIN)) >> BUTTON_PIN)

enum State {
  State_Unpressed = 0,
  State_Pressed
};

static uint8_t button_enabled;
static volatile enum State button_state;
static uint8_t pin_value;

// Sets up the button and its LED
uint8_t button_init(void) {
  // Turn on the pullup resistors
  BUTTON_PORT |= (1 << BUTTON_PIN);
  BUTTON_LED_PORT |= (1 << BUTTON_LED_PIN);

  // Set the button's pin as an input
  BUTTON_DDR |= (0 << BUTTON_PIN);

  // Set the button's LED pin as an output
  BUTTON_LED_DDR |= (1 << BUTTON_LED_PIN);

  // Initialize the button state as being not pressed regardless of its
  // current physical state.
  button_state = State_Unpressed;
  pin_value = NEW_PIN_VALUE;

  button_enabled = 1;

  return button_enabled;
}

// Checks the status of the button and updates its state
void button_update(void) {
  if (!button_enabled) {
    return;
  }

  // Toggle states if the button's pin changed
  if (NEW_PIN_VALUE != pin_value) {

    if (button_state == State_Unpressed) {
      button_state = State_Pressed;
    } else {
      button_state = State_Unpressed;
    }

    pin_value = !pin_value;
  }

  return;
}

// Returns 1 if the button is pressed; 0 otherwise
uint8_t button_is_pressed(void) {
  if (!button_enabled) {
    return 0;
  }

  if (button_state == State_Pressed) {
    return 1;
  }

  return 0;
}

// Turns the button's LED off
void led_turn_off(void) {
  if (!button_enabled) {
    return;
  }

  // I'm not sure why OR'ing with a '0' actually worked. But we're replacing
  // the confusing syntax.
  //BUTTON_LED_PINVEC |= (0 << BUTTON_LED_PIN);
  BUTTON_LED_PORT &= (0 << BUTTON_LED_PIN);

  return;
}

// Turns the button's LED on
void led_turn_on(void) {
  if (!button_enabled) {
    return;
  }

  // According to the note Atmel datasheet Section 14.2.2, writing a one to the
  // pin register provides toggles the value of PORTxn. But that syntax is
  // confusing. We should opt for writing a '1' to turn ON and writing a '0'
  // to turn OFF.
  //BUTTON_LED_PINVEC |= (1 << BUTTON_LED_PIN);
  BUTTON_LED_PORT |= (1 << BUTTON_LED_PIN);

  return;
}

