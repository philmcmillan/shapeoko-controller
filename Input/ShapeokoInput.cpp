#include "ShapeokoInput.h"

#define KEYBUFFER_LENGTH 3

void ShapeokoInput::init(int up, int down, int go, int pause, int abort, Keypad* const pad)
{
  keypad = pad;
  upButton = Bounce();
  upButton.attach(up);
  downButton = Bounce();
  downButton.attach(down);
  goButton = Bounce();
  goButton.attach(go);
  pauseButton = Bounce();
  pauseButton.attach(pause);
  abortButton = Bounce();
  abortButton.attach(abort);
}

ShapeokoCommandButtons ShapeokoInput::checkForInput()
{
  bool stateChanged = abortButton.update();
  int buttonState = abortButton.read();
  if(stateChanged && buttonState == LOW) return ABORT;

  stateChanged = pauseButton.update();
  buttonState = pauseButton.read();
  if(stateChanged && buttonState == LOW) return PAUSE;

  stateChanged = goButton.update();
  buttonState = goButton.read();
  if(stateChanged && buttonState == LOW) return GO;

  stateChanged = upButton.update();
  buttonState = upButton.read();
  if(stateChanged && buttonState == LOW) return UP;

  stateChanged = downButton.update();
  buttonState = downButton.read();
  if(stateChanged && buttonState == LOW) return DOWN;

  char key = keypad->getKey();

  if(key)
  {
    switch(key)
    {
      case KeyOk:
        return OK;
      case KeyCancel:
        clearKeypadBuffer();
        return CANCEL;
      default:
        updateKeypadBuffer(key);
        return NONE;
    }
  }

  return NONE;
}

char* ShapeokoInput::getKeypadBuffer()
{
    return keypad_buffer;
}

void ShapeokoInput::clearKeypadBuffer()
{
  for(int index = 0; index < KEYBUFFER_LENGTH; index++){
    keypad_buffer[index] = Key0;
  }
}

void ShapeokoInput::updateKeypadBuffer(const char key)
{
  for(int index = 1; index < KEYBUFFER_LENGTH; index++)
  {
    keypad_buffer[index -1] = keypad_buffer[index];
  }
  keypad_buffer[2] = key;
}
