#ifndef SHAPEOKO_STATE_MACHINE_H
#define SHAPEOKO_STATE_MACHINE_H

#include <Arduino.h>
#include <SdFat.h>
#include <LiquidCrystal.h>
#include <ShapeokoMenu.h>
#include <ShapeokoInput.h>
#include <ShapeokoPins.h>
#include "ShapeokoState.h"
#include "SendBuffer.h"

class ShapeokoStateProcessorBase{
  public:
    ShapeokoStateProcessorBase(
        LiquidCrystal* const lcd,
        ShapeokoMenu* const menu,
        ShapeokoInput* const input,
        SdFat* const sd){
          this->lcd = lcd;
          this->menu = menu;
          this->input = input;
          this->sd = sd;
        }
    virtual ~ShapeokoStateProcessorBase(){};
    virtual void ProcessState(ShapeokoState* const currentState) = 0;

  protected:
    LiquidCrystal* lcd;
    ShapeokoMenu* menu;
    ShapeokoInput* input;
    SdFat* sd;
};

class MainMenuProcessor : public ShapeokoStateProcessorBase{
  public:
    MainMenuProcessor(LiquidCrystal* lcd, ShapeokoMenu* menu, ShapeokoInput* input, SdFat* sd);
    inline virtual void ProcessState(ShapeokoState* const currentState);

  private:
    ShapeokoProcessorState getNextStateFromMenuChoice(char* const menuChoice);
};

class FileChoiceProcessor : public ShapeokoStateProcessorBase{
  public:
    FileChoiceProcessor(LiquidCrystal* lcd, ShapeokoMenu* menu, ShapeokoInput* input, SdFat* sd);
    ~FileChoiceProcessor();
    inline virtual void ProcessState(ShapeokoState* const currentState);

  private:
    char** fileMenuWindow;
    int getFileCount();
    void updateFileWindow();
    char fileNameBuffer[13];
    void getFileName(const int fileNumber, char* fileName);
    bool setSelectedFile(ShapeokoState* const currentState);
};

class GCodeSendProcessor : public ShapeokoStateProcessorBase{
  public:
    inline virtual void ProcessState(ShapeokoState* const currentState);
    GCodeSendProcessor(LiquidCrystal* lcd, ShapeokoMenu* menu, ShapeokoInput* input, SdFat* sd);
    inline void Reset(){
      if(fileToSend.isOpen()) fileToSend.close();
      refreshMillis = 0;
      outBuffer[0] = '\0';
      fileToSendLineCount = 0;
      fileToSendLinesSent = 0;
      ncCodeLinesProcessed = 0;
      sendQueue.clear();
      inBuffer[0] = '\0';
      inBufferPosition = 0;
    }

  private:
    SdFile fileToSend;
    ShapeokoProcessorState prepareForSending(const char* filename);
    ShapeokoProcessorState sendNextGCodeLine();
    ShapeokoProcessorState checkForGRBLResponse();
    ShapeokoProcessorState processGRBLLine();
    ShapeokoProcessorState waitForFinish();
    void updateDisplay();

    int refreshMillis;
    char outBuffer[80];
    int fileToSendLineCount;
    int fileToSendLinesSent;
    int ncCodeLinesProcessed;
    SendBuffer sendQueue;
    static ArduinoOutStream ncStream;
    static const int SEND_BUFFER = 128;
    char inBuffer[80];
    int inBufferPosition;
};

#endif
