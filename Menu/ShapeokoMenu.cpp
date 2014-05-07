#include <LiquidCrystal.h>
#include "ShapeokoMenu.h"

const char main_menu_1[] PROGMEM = "Exec NC File";
const char main_menu_2[] PROGMEM = "Jog";
const char main_menu_3[] PROGMEM = "Reset";

PGM_P main_menu[] PROGMEM = {
  main_menu_1,
  main_menu_2,
  main_menu_3
};

ShapeokoMenu::ShapeokoMenu(int windowRows)
{
  menuRows = windowRows;
  menuPosition = 0;
}

ShapeokoMenu::~ShapeokoMenu()
{
  menuRows = 0;
  menuPosition = 0;
}

void ShapeokoMenu::incrementMenu()
{
  menuPosition++;
}

void ShapeokoMenu::decrementMenu()
{
  if(menuPosition > 0) menuPosition--;
}

int ShapeokoMenu::getCurrentMenuPosition(){
  return this->menuPosition;
}

void ShapeokoMenu::resetMenuPosition(){
  this->menuPosition = 0;
}

int ShapeokoMenu::getMenuWindowSize(){
  return this->menuRows;
}

void ShapeokoMenu::renderMenu(ShapeokoMenus menu, LiquidCrystal* const lcd, char* const files[])
{
  switch(menu)
  {
    case MAIN:
      renderMain(lcd);
      break;
    case JOG:
      renderMain(lcd); //TBD
      break;
    case FILES:
      renderFiles(lcd, files);
      break;
  };
}

void ShapeokoMenu::renderMain(LiquidCrystal* const lcd)
{
  char buffer[13];
  lcd->clear();
  lcd->print(F("Main Menu"));

  for(int currentRow = menuPosition; currentRow <= menuRows; currentRow++)
  {
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(main_menu[menuPosition])));
    lcd->setCursor(0,currentRow);
    lcd->print(menuPosition + 1);
    lcd->print(F(". "));
    lcd->print(buffer);
  }
}

void ShapeokoMenu::renderFiles(LiquidCrystal* const lcd, char* const file_window[])
{
  lcd->clear();
  lcd->print(F("Select file:"));

  if(file_window == NULL){
    this->menuPosition = 0;
    lcd->setCursor(0,1);
    lcd->print(F("Insert card..."));
  }

  for(int currentRow = 1; currentRow <= menuRows; currentRow++)
  {
    lcd->setCursor(0,currentRow);
    lcd->print(menuPosition + 1);
    lcd->print(F(". "));
    lcd->print(file_window[currentRow - 1]);
  }
}
