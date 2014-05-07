#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Bounce2.h>
#include <SdFat.h>
#include <ShapeokoMenu.h>
#include <ShapeokoInput.h>
#include <ShapeokoStateMachine.h>
#include <ShapeokoPins.h>
#include <SPI.h>
#include "ShapeokoController.h"

volatile bool serialOverride = false;
LiquidCrystal lcd = LiquidCrystal(22,23,24,25,26,27);
ShapeokoMenu menu = ShapeokoMenu(2);
ShapeokoInput input = ShapeokoInput();
Keypad kpad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
ShapeokoState currentState = ShapeokoState();
SdFat sd;

FileChoiceProcessor fcp(&lcd, &menu, &input, &sd);
MainMenuProcessor mmp(&lcd, &menu, &input, &sd);
GCodeSendProcessor gcsp(&lcd, &menu, &input, &sd);

void setup()
{
  lcd.begin(20,4);
  lcd.clear();
  lcd.print("Booting...");
  Serial.begin(19200);
  Serial1.begin(9600);
  SdFat::setStdOut(&lcd);
  setUpPinState();
  setUpBlinker();
  input.init(2,3,4,40,41, &kpad);
  currentState.setProcessorState(MAIN_MENU);
}

void serialEvent(){
  if(currentState.getProcessorState() < START_GCODE_STREAM)
    serialOverride = true;
}

void loop(){
  checkFanState();
  if(serialOverride){
    lcd.clear();
    lcd.print("Serial override");
    while(Serial.available()){
      char c = Serial.read();
      Serial1.write(c);
    }

    while(Serial1.available()){
      char c = Serial1.read();
      Serial.write(c);
    }
    if(input.checkForInput() == OK){
      serialOverride = false;
    }
  }
  else{
    switch(currentState.getProcessorState()){
      case MAIN_MENU:
        mmp.ProcessState(&currentState);
        break;
      case FILE_CHOOSER:
        fcp.ProcessState(&currentState);
        break;
      case START_GCODE_STREAM:
      case GCODE_STREAMING:
      case WAITING_ON_GRBL:
      case WAITING_ON_FINISH:
        gcsp.ProcessState(&currentState);
        break;
      case ERROR:
        digitalWrite(PIN_ERROR_LED, LOW);
      case SEND_COMPLETE:
        digitalWrite(PIN_DATA_LED, HIGH);
        lcd.setCursor(0,3);
        lcd.print("Press OK To Continue");
        if(input.checkForInput() == OK)
          currentState.setProcessorState(RESETTING);
        break;
      case RESETTING:
        currentState.setNumberOfAvailableFiles(0);
        currentState.selectedFilename()[0] = '\0';
        currentState.setProcessorState(RESET);
        gcsp.Reset();
      case RESET:
        digitalWrite(PIN_ERROR_LED, HIGH);
        currentState.setProcessorState(MAIN_MENU);
    }
  }
}

void setUpPinState(){
    pinMode(PIN_MOTOR_ENABLE, INPUT_PULLUP);
    pinMode(PIN_CD, INPUT_PULLUP);
    pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_GO, INPUT_PULLUP);
    pinMode(PIN_BUTTON_PAUSE, INPUT_PULLUP);
    pinMode(PIN_BUTTON_ABORT, INPUT_PULLUP);
    pinMode(PIN_MOTOR_ENABLE, INPUT_PULLUP);

    pinMode(PIN_SD_SS, OUTPUT);
    pinMode(PIN_BLINK_SS, OUTPUT);
    pinMode(PIN_DATA_LED, OUTPUT);
    pinMode(PIN_ERROR_LED, OUTPUT);
    pinMode(PIN_FAN_OUT_1, OUTPUT);
    pinMode(PIN_FAN_OUT_2, OUTPUT);
}

void setUpBlinker(){
  SPI.begin();
  digitalPotWrite(0, 165);
  digitalPotWrite(1, 70);
  SPI.end();
}

void digitalPotWrite(int address, int value){
   digitalWrite(PIN_SD_SS, HIGH);
   digitalWrite(PIN_BLINK_SS, LOW);
   SPI.transfer(address);
   SPI.transfer(value);
   digitalWrite(PIN_BLINK_SS, HIGH);
}

void checkFanState(){
  bool fanOn = !digitalRead(PIN_MOTOR_ENABLE);
  digitalWrite(PIN_FAN_OUT_1, fanOn);
  digitalWrite(PIN_FAN_OUT_2, fanOn);
}
