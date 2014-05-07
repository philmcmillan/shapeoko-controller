#ifndef SHAPEOKO_MENU_H
#define SHAPEOKO_MENU_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

enum ShapeokoMenus {
  MAIN,
  FILES,
  JOG
};

class ShapeokoMenu {
  public:
    ShapeokoMenu (int windowRows);
    ~ShapeokoMenu();

    void incrementMenu();
    void decrementMenu();
    int getCurrentMenuPosition();
    int getMenuWindowSize();
    void renderMenu(ShapeokoMenus menu, LiquidCrystal* const lcd, char* const files[]);
    void resetMenuPosition();

  private:
    int menuPosition;
    int menuRows;

    void renderMain(LiquidCrystal* const lcd);
    void renderFiles(LiquidCrystal* const lcd, char* const files[]);

};

#endif
