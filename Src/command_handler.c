#include "command_handler.h"

uint8_t stack[MAX_STACK_SIZE] = {0};
RotaryInteractionConfig rotary_configs[MAX_ROTARY] = {0};
uint32_t size = 0;


void Init_Rotary_Action_Config(RotaryActionConfig *action_config)
{
  action_config->Keyboard.Id = 1;
  action_config->Media.Id = 2;
}

void Init_Command_Handler()
{
  uint8_t i = 0;
  for(i = 0; i < MAX_ROTARY; i++)
  {
    RotaryInteractionConfig interactionConfig = {0};
    Init_Rotary_Action_Config(&interactionConfig.Clockwise);
    Init_Rotary_Action_Config(&interactionConfig.Counterclockwise);
    Init_Rotary_Action_Config(&interactionConfig.Pressed);
    Init_Rotary_Action_Config(&interactionConfig.Released);
    interactionConfig.Clockwise.Mode = MEDIA_MODE;
    interactionConfig.Clockwise.Media.Key = USB_HID_VOL_UP;
    interactionConfig.Counterclockwise.Mode = MEDIA_MODE;
    interactionConfig.Counterclockwise.Media.Key = USB_HID_VOL_DEC;
    rotary_configs[i] = interactionConfig;
  }
}

void Send_Greetings()
{
  Send_Command_Mail(ebCONNECTED, NULL, NULL);
}

void Process_Command_Input(uint8_t* Buf, uint32_t *Len)
{
  uint32_t i;
  for (i = 0; i < Len; i++)
  {
    uint8_t lastValue =  *Buf;
    if (lastValue == '\n' || lastValue == '\r')
    {
      if(Parse_Command())
      {
        Send_Command_Mail(ebCOMMAND, NULL, NULL);
        Clear_Command_Stack();
      }
      else
      {
        Send_Command_Mail(ebERROR, NULL, NULL);
        Clear_Command_Stack();
      }
    }
    else if(size >= MAX_STACK_SIZE - 1)
    {
      Send_Command_Mail(ebOVERFLOWN, NULL, NULL);
      Clear_Command_Stack();
    }
    else
    {
      stack[size] = lastValue;
      size++;
      Send_Command_Mail(ebWAITING, Buf, Len);
    }
  }
}

uint8_t Get_Command_Value(uint8_t *out, uint8_t start, uint8_t size, uint8_t base)
{
  char *endptr;
  uint8_t substack[size+1];
  uint8_t value;
  errno = 0;
  memcpy(substack, stack + start, sizeof(*stack) * size);
  substack[size] = 0;
  value = (uint8_t)strtol(substack, &endptr, base);
  if ( errno == ERANGE
    || *endptr != '\0'
  ) {
    return -1;
  }
  *out = value;
  return 1;
}

uint8_t Parse_Command()
{
  uint8_t rotaryId;
  RotaryActionConfig *action_config;
  uint8_t key;
  if (!Get_Command_Value(&rotaryId, 0, 1, 10))
  {
    return 0;
  }
  if (rotaryId > MAX_ROTARY)
  {
    return 0;
  }
  switch(stack[1])
  {
    case 'R':
    case 'r':
      action_config = &rotary_configs[rotaryId].Released;
      break;
    case 'P':
    case 'p':
      action_config = &rotary_configs[rotaryId].Pressed;
      break;
    case 'C':
    case 'c':
      action_config = &rotary_configs[rotaryId].Clockwise;
      break;
    case 'X':
    case 'x':
      action_config = &rotary_configs[rotaryId].Counterclockwise;
      break;
    default:
      return 0;
      break;
  }
  switch(stack[2])
  {
    case 'K':
    case 'k':
      if (size != 11)
      {
        return 0;
      }
      action_config->Mode = KEYBOARD_MODE;
      if (!Get_Command_Value(&key, 3, 2, 16))
      {
        return 0;
      }
      action_config->Keyboard.Modifiers = key;
      if (!Get_Command_Value(&key, 5, 2, 16))
      {
        return 0;
      }
      action_config->Keyboard.Key1 = key;
      if (!Get_Command_Value(&key, 7, 2, 16))
      {
        return 0;
      }
      action_config->Keyboard.Key2 = key;
      if (!Get_Command_Value(&key, 9, 2, 16))
      {
        return 0;
      }
      action_config->Keyboard.Key3 = key;
      break;
    case 'M':
    case 'm':
      if (size != 5)
      {
        return 0;
      }
      action_config->Mode = MEDIA_MODE;
      if (!Get_Command_Value(&key, 3, 2, 16))
      {
        return 0;
      }
      action_config->Media.Key = key;
      break;
    default:
      return 0;
      break;
  }
  return 1;
}

void Clear_Command_Stack()
{
  size = 0;
  uint32_t i;
  for (i = 0; i < MAX_STACK_SIZE; i++)
  {
    stack[i] = 0;
  }
}

void Send_Command_Mail(uint16_t EventCode, uint8_t* Buf, uint32_t Len)
{
  CommandMail *mptr;
  mptr = osMailAlloc(commandMailQ, osWaitForever);
  if (mptr == NULL)
  {
    Error_Handler();
    return;
  }
  memcpy(mptr->Command, stack, MAX_STACK_SIZE);
  if (Buf != NULL)
  {
    mptr->LastBuf = Buf;
  }
  if (Len)
  {
    mptr->LastLen = Len;
  }
  mptr->EventCode = EventCode;
  if(osMailPut(commandMailQ, mptr) != osOK)
  {
    Error_Handler();
  }
  if(osThreadYield() != osOK)
  {
    Error_Handler();
  }
}
