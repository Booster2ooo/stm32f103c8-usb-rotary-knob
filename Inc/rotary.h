#include "gpio.h"

// From freertos.c
extern osThreadId defaultTaskHandle;

// FreeRTOS Event Bits
#define ebPRESSED 0x1
#define ebRELEASED 0x2
#define ebCLOCKWISE 0x4
#define ebCOUNTERCLOCKWISE 0x8
#define ROTARYID1 0x00
#define ROTARYID2 0x10
#define ROTARYID3 0x20
#define ROTARYID4 0x40
#define ROTARYID5 0x80

typedef struct
{
  uint16_t Id;
  GPIO_TypeDef *ClkPort;
  GPIO_TypeDef *DtPort;
  GPIO_TypeDef *SwPort;
  uint32_t ClkPin;
  uint32_t DtPin;
  uint32_t SwPin;
  uint8_t SamplesCount;
  float FilterOutput;
  int SwStableState;
  GPIO_PinState SwPhysicalState;
  uint8_t RotaryState;
  uint16_t RotaryStateStore;
} RotaryEncoderTypeDef;

typedef struct
{
	RotaryEncoderTypeDef *Rotary;
	uint16_t EventCode;
} RotaryMail;

extern RotaryEncoderTypeDef rotary1;
extern RotaryEncoderTypeDef rotary2;
extern RotaryEncoderTypeDef rotary3;
extern RotaryEncoderTypeDef rotary4;
extern RotaryEncoderTypeDef rotary5;

extern osMailQId rotaryMailQ;

void Init_Rotary(RotaryEncoderTypeDef *rotary, uint16_t id, GPIO_TypeDef *clk_port, uint32_t clk_pin, GPIO_TypeDef *dt_port, uint32_t dt_pin, GPIO_TypeDef *sw_port, uint32_t sw_pin);
void Debounce_Switch(RotaryEncoderTypeDef *rotary);
int8_t Read_Rotary(RotaryEncoderTypeDef *rotary);
void Compute_Rotary(RotaryEncoderTypeDef *rotary);
void Send_Mail(RotaryEncoderTypeDef *rotary, uint16_t EventCode);
