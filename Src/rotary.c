#include "rotary.h"

RotaryEncoderTypeDef rotary1 = {0};
RotaryEncoderTypeDef rotary2 = {0};
RotaryEncoderTypeDef rotary3 = {0};
RotaryEncoderTypeDef rotary4 = {0};
RotaryEncoderTypeDef rotary5 = {0};

void Init_Rotary(RotaryEncoderTypeDef *rotary, uint16_t id, GPIO_TypeDef *clk_port, uint32_t clk_pin, GPIO_TypeDef *dt_port, uint32_t dt_pin, GPIO_TypeDef *sw_port, uint32_t sw_pin)
{
  rotary->Id = id;
  rotary->ClkPort = clk_port;
  rotary->ClkPin = clk_pin;
  rotary->DtPort = dt_port;
  rotary->DtPin = dt_pin;
  rotary->SwPort = sw_port;
  rotary->SwPin = sw_pin;
  rotary->SamplesCount = 5;
  rotary->FilterOutput = 1.0;
  rotary->SwStableState = 1.0;
  rotary->SwPhysicalState = 1.0;
  rotary->RotaryState = 0;
  rotary->RotaryStateStore = 0;
}

void Debounce_Switch(RotaryEncoderTypeDef *rotary)
{
  rotary->SwPhysicalState = HAL_GPIO_ReadPin(rotary->SwPort, rotary->SwPin);
  // virtual capacitor charge/discharge emulation
  rotary->FilterOutput = ((float)rotary->SwPhysicalState + (float)rotary->SamplesCount / 3.0 * rotary->FilterOutput)/(1 + (float)rotary->SamplesCount / 3.0);
  if ((rotary->FilterOutput > 0.95 && rotary->SwStableState == GPIO_PIN_RESET)
   || (rotary->FilterOutput < 0.05 && rotary->SwStableState == GPIO_PIN_SET))
  {
    rotary->SwStableState = rotary->FilterOutput > 0.95 ? GPIO_PIN_SET : GPIO_PIN_RESET;
    rotary->FilterOutput = (float)rotary->SwStableState;
    Send_Rotary_Mail(rotary, rotary->SwStableState ? ebPRESSED : ebRELEASED);
  }
}

int8_t Read_Rotary(RotaryEncoderTypeDef *rotary)
{
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};
  rotary->RotaryState <<= 2;
  if (HAL_GPIO_ReadPin(rotary->DtPort, rotary->DtPin))
  {
    rotary->RotaryState |= 0x02;
  }
  if (HAL_GPIO_ReadPin(rotary->ClkPort, rotary->ClkPin))
  {
    rotary->RotaryState |= 0x01;
  }
  rotary->RotaryState &= 0x0f;
  if  (rot_enc_table[rotary->RotaryState] ) 
  {
    rotary->RotaryStateStore <<= 4;
    rotary->RotaryStateStore |= rotary->RotaryState;
    if ((rotary->RotaryStateStore & 0xff) == 0x2b) 
    {
      return 1;
    }
    if ((rotary->RotaryStateStore & 0xff) == 0x17)
    {
      return -1;
    }
  }
  return 0;
}

void Compute_Rotary(RotaryEncoderTypeDef *rotary)
{
  if(Read_Rotary(rotary))
  {
    if (rotary->RotaryState == 0x0b) 
    {
      Send_Rotary_Mail(rotary, ebCLOCKWISE);
    }
    if (rotary->RotaryState == 0x07) 
    {
      Send_Rotary_Mail(rotary, ebCOUNTERCLOCKWISE);
    }
  }
}

void Send_Rotary_Mail(RotaryEncoderTypeDef *rotary, uint16_t EventCode)
{
  RotaryMail *mptr;
  mptr = osMailAlloc(rotaryMailQ, osWaitForever);
  if (mptr == NULL)
  {
    Error_Handler();
    return;
  }
  mptr->Rotary = rotary;
  mptr->EventCode = EventCode;
  if(osMailPut(rotaryMailQ, mptr) != osOK)
  {
    Error_Handler();
  }
  if(osThreadYield() != osOK)
  {
    Error_Handler();
  }
}
