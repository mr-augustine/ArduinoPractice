/*
 * File: main.c
 */
#include <stdio.h>

#include "statevars.h"

statevars_t statevars;

int main(void) {
  memset(&statevars, 0, sizeof(statevars));

  return;
}

