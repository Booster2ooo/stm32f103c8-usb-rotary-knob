
#include "mediakeyboard.h"

KeyboardHIDTypeDef keyboardHID = {0};
MediaHIDTypeDef mediaHID = {0};

void Init_Mediakeyboard() {
  // Define report ids
  keyboardHID.Id = 1;
  mediaHID.Id = 2;
}
