#include "ShapeokoStateMachine.h"

const char* GCODE_EXT = ".NC";

FileChoiceProcessor::FileChoiceProcessor(LiquidCrystal* lcd, ShapeokoMenu* menu, ShapeokoInput* input, SdFat* sd)
  :ShapeokoStateProcessorBase(lcd, menu, input, sd){
  int menuWindow = menu->getMenuWindowSize();
  this->fileMenuWindow = new char* [menuWindow];

  for(int index = 0; index < menuWindow; index++){
    this->fileMenuWindow[index] = new char[13];
  }
}

FileChoiceProcessor::~FileChoiceProcessor(){
  for(int index = 0; index < menu->getMenuWindowSize(); index++){
    delete this->fileMenuWindow[index];
  }
  delete this->fileMenuWindow;
}

void FileChoiceProcessor::ProcessState(ShapeokoState* const currentState){

  if(digitalRead(PIN_CD) != LOW){
    menu->renderMenu(FILES, this->lcd, NULL);
    currentState->setNumberOfAvailableFiles(0);
    return;
  }

  if(currentState->getNumberOfAvailableFiles() == 0){
    menu->resetMenuPosition();
    currentState->setNumberOfAvailableFiles(getFileCount());
    return;
  }

  this->updateFileWindow();
  menu->renderMenu(FILES, this->lcd, this->fileMenuWindow);

  switch(input->checkForInput()){
    case UP:
      menu->decrementMenu();
      break;
    case DOWN:
      if(menu->getCurrentMenuPosition() < (currentState->getNumberOfAvailableFiles() - menu->getMenuWindowSize())){
        menu->incrementMenu();
      }
      break;
    case CANCEL:
      menu->resetMenuPosition();
      currentState->setProcessorState(MAIN_MENU);
      break;
    case OK:
      if(setSelectedFile(currentState))
        currentState->setProcessorState(START_GCODE_STREAM);
      break;
  }
}

bool FileChoiceProcessor::setSelectedFile(ShapeokoState* const currentState){
    int selectedFileNum = atoi(input->getKeypadBuffer());
    if(selectedFileNum ==0 || selectedFileNum >= currentState->getNumberOfAvailableFiles()){
      input->clearKeypadBuffer();
      return false;
    }
    this->getFileName(selectedFileNum - 1, currentState->selectedFilename());
    return true;
}

void FileChoiceProcessor::updateFileWindow(){
  if(!sd->begin(PIN_SD_SS, SPI_FULL_SPEED)){
    lcd->clear();
    sd->initErrorPrint();
  }
  int menuPosition = menu->getCurrentMenuPosition();
  for(int index = 0; index < menu->getMenuWindowSize(); index++)
  {
    getFileName(menuPosition + index, this->fileMenuWindow[index]);
  }
}

void FileChoiceProcessor::getFileName(const int fileNumber, char* fileName){
  int counter = 0;
  if(!sd->begin(PIN_SD_SS, SPI_FULL_SPEED)){
    lcd->clear();
    sd->initErrorPrint();
  }
  sd->vwd()->rewind();
  SdFile file;
  while(counter < fileNumber && file.openNext(sd->vwd(), O_READ)){
    if(file.isFile()){
      file.getFilename(fileNameBuffer);
      String fileNameToCompare = String(fileNameBuffer);
      if(!fileNameToCompare.startsWith("_") && fileNameToCompare.endsWith(GCODE_EXT)){
        if(counter == fileNumber){
          strcpy(fileName, fileNameBuffer);
          return;
        }
        counter++;
      }
    }
  }
}

int FileChoiceProcessor::getFileCount(){
  int fileCount = 0;
  if(!sd->begin(PIN_SD_SS, SPI_FULL_SPEED)){
    lcd->clear();
    sd->initErrorPrint();
  }

  sd->vwd()->rewind();
  SdFile file;
  while (file.openNext(sd->vwd(), O_READ))
  {
    if (file.isFile())
    {
      file.getFilename(fileNameBuffer);
      String fileNameToCompare = String(fileNameBuffer);
      if (!fileNameToCompare.startsWith("_") && fileNameToCompare.endsWith(GCODE_EXT))
      {
        fileCount++;
      }
    }
    file.close();
  }

  return fileCount;
}
