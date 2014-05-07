#ifndef SHAPEOKO_CTRL_H
#define SHAPEOKO_CTRL_H

#include <ShapeokoInput.h>

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {Key6,Key5,Key6},
  {Key1,Key2,Key3},
  {Key7,Key8,Key9},
  {KeyOk,Key0,KeyCancel}
  };

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10 };

#endif
