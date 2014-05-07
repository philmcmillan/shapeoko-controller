#include "ShapeokoStateMachine.h"

ArduinoOutStream GCodeSendProcessor::ncStream(Serial1);

GCodeSendProcessor::GCodeSendProcessor(LiquidCrystal* lcd, ShapeokoMenu* menu, ShapeokoInput* input, SdFat* sd)
  :ShapeokoStateProcessorBase(lcd, menu, input, sd){

  fileToSendLineCount = 0;
  fileToSendLinesSent = 0;
  inBufferPosition = 0;
  outBuffer[0] = '\0';
  inBuffer[0] = '\0';
}

void GCodeSendProcessor::ProcessState(ShapeokoState* const currentState){

  if(input->checkForInput() == ABORT)
  {
    lcd->clear();
    lcd->print("Send aborted.");
    currentState->setProcessorState(ERROR);
    return;
  }

  switch(currentState->getProcessorState())
  {
    case START_GCODE_STREAM:
      currentState->setProcessorState(prepareForSending(currentState->selectedFilename()));
      break;
    case GCODE_STREAMING:
      currentState->setProcessorState(sendNextGCodeLine());
      break;
    case WAITING_ON_GRBL:
      currentState->setProcessorState(checkForGRBLResponse());
      break;
    case WAITING_ON_FINISH:
      currentState->setProcessorState(waitForFinish());
    default:
      lcd->clear();
      lcd->print(F("Error: unkown state during send"));
      currentState->setProcessorState(ERROR);
      break;
  }
}

void GCodeSendProcessor::updateDisplay(){
  if((millis() - refreshMillis) > 500)
  {
    lcd->clear();
    lcd->print(F("Sent "));
    lcd->print(fileToSendLinesSent);
    lcd->print(F(" of "));
    lcd->print(fileToSendLineCount);
    lcd->setCursor(0, 1);
    lcd->print(F("Compl "));
    lcd->print(ncCodeLinesProcessed);
    lcd->print(F(" of "));
    lcd->print(fileToSendLineCount);

    refreshMillis = millis();
  }
}

ShapeokoProcessorState GCodeSendProcessor::waitForFinish(){
  while(Serial1.available())
    {
      char c = Serial1.read();
      if(c == '\n')
      {
        sendQueue.pop();
        this->ncCodeLinesProcessed++;
        if(sendQueue.isEmpty())
        {
          lcd->clear();
          lcd->print(F("File complete"));
          if(this->fileToSend.isOpen()) this->fileToSend.close();
          return SEND_COMPLETE;
        }
      }
    }
    updateDisplay();
    return WAITING_ON_FINISH;
}

ShapeokoProcessorState GCodeSendProcessor::sendNextGCodeLine(){
  int n = 0;
  if((n = this->fileToSend.fgets(outBuffer, sizeof(outBuffer))) > 0)
  {
    sendQueue.push(n);
    if(sendQueue.sum() >= SEND_BUFFER)
    {
      return WAITING_ON_GRBL;
    }

    GCodeSendProcessor::ncStream << outBuffer;
    fileToSendLinesSent++;
    return GCODE_STREAMING;
  }

  return WAITING_ON_FINISH;
}

ShapeokoProcessorState GCodeSendProcessor::checkForGRBLResponse()
{
  while(Serial1.available())
  {
    char c = Serial1.read();
    if(c == '\r')
      continue;

    inBuffer[inBufferPosition++] = c;
    inBuffer[inBufferPosition] = '\0';

    if( c == '\n' ){
      ShapeokoProcessorState resultState =  processGRBLLine();
      inBufferPosition = 0;
      inBuffer[inBufferPosition] = '\0';
      return resultState;
    }
  }

  return WAITING_ON_GRBL;
}

ShapeokoProcessorState GCodeSendProcessor::processGRBLLine()
{
  if ((strncmp(inBuffer, "ok",2) != 0) && (strncmp(inBuffer, "error",5) != 0))
  {
    lcd->clear();
    lcd->print(F("Unknown Response:"));
    lcd->setCursor(0, 1);
    lcd->print(inBuffer);
    return ERROR;
  }
  else
  {
    sendQueue.pop();
    this->ncCodeLinesProcessed++;

    if(sendQueue.sum() < SEND_BUFFER && outBuffer[0] != '\0')
    {
      ncStream << outBuffer;
      this->fileToSendLinesSent++;
      outBuffer[0] = '\0';
      return GCODE_STREAMING;
    }

    return WAITING_ON_GRBL;
  }
}

ShapeokoProcessorState GCodeSendProcessor::prepareForSending(const char* fileNameToSend){
  lcd->clear();
  lcd->print(F("Loading file"));
  lcd->setCursor(0,1);
  lcd->print(fileNameToSend);
  lcd->print(F("..."));

  if(!sd->begin(PIN_SD_SS, SPI_FULL_SPEED)){
    lcd->clear();
    sd->initErrorPrint();
    return ERROR;
  }

  if(!sd->vwd()->exists(fileNameToSend)){
    lcd->clear();
    lcd->print(F("File not found."));
    return ERROR;
  }

  if(!this->fileToSend.open(sd->vwd(), fileNameToSend, O_READ))
  {
    lcd->clear();
    lcd->print(F("Can't open file."));
    lcd->setCursor(0, 1);
    sd->errorPrint();
    return ERROR;
  }

  fileToSendLineCount = 0;
  inBufferPosition = 0;
  outBuffer[0] = '\0';
  inBuffer[0] = '\0';

  // get length of file in gcode lines
  while(this->fileToSend.fgets(this->outBuffer, sizeof(this->outBuffer), (char*)F("\n")) > 0){
    this->fileToSendLineCount++;
  }
  this->fileToSend.rewind();
  this->fileToSendLinesSent = 0;

  digitalWrite(PIN_DATA_LED, LOW);

  return GCODE_STREAMING;
}
