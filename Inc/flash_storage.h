#include <stdint.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#define FLASH_END_ADDRESS			((uint32_t)0x08010000)
#define FLASH_STORAGE_START_ADDRESS		FLASH_END_ADDRESS - (FLASH_PAGE_SIZE)

void Write_Flash_Config(uint32_t address, void *config_ptr, uint8_t size);
void Read_Flash_Config(uint32_t address, void *config_ptr, uint8_t size);
