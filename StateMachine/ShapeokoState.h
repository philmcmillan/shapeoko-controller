#ifndef SHAPEOKO_STATE_H
#define SHAPEOKO_STATE_H

#include <SdFat.h>

enum ShapeokoProcessorState{
  MAIN_MENU           = 0,
  FILE_CHOOSER        = 1,
  START_GCODE_STREAM  = 2,
  GCODE_STREAMING     = 3,
  WAITING_ON_GRBL     = 4,
  WAITING_ON_FINISH   = 5,
  SEND_COMPLETE       = 6,
  RESETTING           = 7,
  RESET               = 8,
  ERROR               = 9,
};

class ShapeokoState{
  public:
    inline ShapeokoProcessorState getProcessorState(){ return currentProcessorState; }
    inline void setProcessorState(const ShapeokoProcessorState processorState){ currentProcessorState = processorState;}
    inline int getNumberOfAvailableFiles(){ return numFilesOnCard; }
    inline void setNumberOfAvailableFiles(const int numFiles){ numFilesOnCard = numFiles; }
    inline char* const selectedFilename(){ return selectedFile; }

  private:
    ShapeokoProcessorState currentProcessorState;
    int numFilesOnCard;
    char selectedFile[13];
};

#endif
