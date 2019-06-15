#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cmsis_os.h"
#include "rotary.h"
#include "mediakeyboard.h"

#define MAX_STACK_SIZE 64
#define ebERROR						0x0
#define ebCONNECTED				0x1
#define ebWAITING					0x2
#define ebCOMMAND					0x4
#define ebOVERFLOWN				0x8
#define KEYBOARD_MODE			0x1
#define MEDIA_MODE				0x2

typedef struct
{
  uint8_t Command[MAX_STACK_SIZE];
  uint8_t LastValue;
  uint16_t EventCode;
} CommandMail;

typedef struct
{
  uint8_t Mode;
  KeyboardHIDTypeDef Keyboard;
  MediaHIDTypeDef Media;
} RotaryActionConfig;

typedef struct
{
  RotaryActionConfig Clockwise;
  RotaryActionConfig Counterclockwise;
  RotaryActionConfig Pressed;
  RotaryActionConfig Released;
} RotaryInteractionConfig;

extern uint8_t stack[MAX_STACK_SIZE];
extern osMailQId commandMailQ;
RotaryInteractionConfig rotary_configs[MAX_ROTARY];

void Init_Rotary_Action_Config(RotaryActionConfig *action_config);
void Init_Command_Handler();
uint8_t Get_Command_Value(uint8_t *out, uint8_t start, uint8_t size, uint8_t base);
uint8_t Parse_Command();
void Send_Greetings();
void Process_Command_Input(uint8_t* Buf, uint32_t *Len);
void Clear_Command_Stack();
void Send_Command_Mail(uint16_t EventCode, uint8_t lastValue);
