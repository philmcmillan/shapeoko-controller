#include "ShapeokoStateMachine.h"

#define CHOICE_FILE   1
#define CHOICE_JOG    2
#define CHOICE_RESET  3

MainMenuProcessor::MainMenuProcessor(LiquidCrystal* lcd, ShapeokoMenu* menu, ShapeokoInput* input, SdFat* sd)
  : ShapeokoStateProcessorBase(lcd, menu, input, sd){}

void MainMenuProcessor::ProcessState(ShapeokoState* const currentState){
  menu->renderMenu(MAIN, lcd, NULL);
  lcd->setCursor(3,0);
  lcd->print(F("Current Selection: "));
  lcd->print(input->getKeypadBuffer());

  ShapeokoCommandButtons button = input->checkForInput();

  switch(button){
    case OK:
      menu->resetMenuPosition();
      currentState->setNumberOfAvailableFiles(0);
      currentState->setProcessorState(getNextStateFromMenuChoice(input->getKeypadBuffer()));
      break;
    case UP:
      menu->incrementMenu();
      break;
    case DOWN:
      menu->decrementMenu();
      break;
    case CANCEL:
      input->clearKeypadBuffer();
      currentState->setProcessorState(MAIN_MENU);
      break;
  }
}

ShapeokoProcessorState MainMenuProcessor::getNextStateFromMenuChoice(char* const menuChoice){
    int menuItem = atoi(menuChoice);

    switch(menuItem){
      case CHOICE_FILE:
        return FILE_CHOOSER;
      case CHOICE_JOG:
        return MAIN_MENU;
      case CHOICE_RESET:
        return RESETTING;
    }

    return MAIN_MENU;
}
