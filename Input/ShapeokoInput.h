#ifndef SHAPEOKO_INPUT_H
#define SHAPEOKO_INPUT_H

#include <Arduino.h>
#include <Keypad.h>
#include <Bounce2.h>

#define Key0 0x30
#define Key1 0x31
#define Key2 0x32
#define Key3 0x33
#define Key4 0x34
#define Key5 0x35
#define Key6 0x36
#define Key7 0x37
#define Key8 0x38
#define Key9 0x39
#define KeyOk 0x03
#define KeyCancel 0x18

enum ShapeokoCommandButtons {
  NONE,
  OK,
  CANCEL,
  GO,
  PAUSE,
  ABORT,
  UP,
  DOWN
};

class ShapeokoInput {
  public:
      void init(int up, int down, int go, int pause, int abort, Keypad* const pad);
      ShapeokoCommandButtons checkForInput();
      char* getKeypadBuffer();
      void clearKeypadBuffer();

  private:
      void updateKeypadBuffer(const char key);
      char keypad_buffer[4];
      Keypad* keypad;
      Bounce upButton;
      Bounce downButton;
      Bounce goButton;
      Bounce pauseButton;
      Bounce abortButton;
};

#endif
