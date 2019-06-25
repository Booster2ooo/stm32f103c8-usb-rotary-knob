#include "flash_storage.h"

FLASH_EraseInitTypeDef EraseInitStruct = {0};
uint32_t PageError = 0;


void Write_Flash_Config(uint32_t address, void *config_ptr, uint8_t size)
{
	uint32_t ret;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_FLASH_Unlock();
	EraseInitStruct.PageAddress = address;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.NbPages     = 1;
	ret = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	if (ret != HAL_OK)
	{
		switch(ret) {
				case HAL_TIMEOUT:
						printf("HAL_FLASHEx_Erase() timeout!\n");
						break;
				case HAL_ERROR:
						printf("HAL_FLASHEx_Erase() error 0x%08x, see *hal_flash.h for bit definitions\n", HAL_FLASH_GetError());
						break;
				default:
						printf("HAL_FLASHEx_Erase() returned unknown status %lu\n", ret);
		}
		HAL_FLASH_Lock();
		Error_Handler();
		return;
	}
	uint16_t *data = config_ptr;
	uint8_t halfsize = size / 2; // sizeof returns bytes units (8bits) but we write halfword chunks (16bits)
	while(halfsize--)
	{
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, *data++);
		address += 2;
		if (ret != HAL_OK)
		{
			switch(ret) {
					case HAL_TIMEOUT:
							printf("HAL_FLASH_Program() timeout!\n");
							break;
					case HAL_ERROR:
							printf("HAL_FLASH_Program() error 0x%08x, see *hal_flash.h for bit definitions\n", HAL_FLASH_GetError());
							break;
					default:
							printf("HAL_FLASH_Program() returned unknown status %lu\n", ret);
			}
			HAL_FLASH_Lock();
			Error_Handler();
			return;
		}
	}
	return;
}

void Read_Flash_Config(uint32_t address, void *config_ptr, uint8_t size)
{
	uint16_t *data = (uint16_t *)address;
	uint16_t *dst = (uint16_t *)config_ptr;
	uint8_t halfsize = size / 2;
	while(halfsize--)
	{
		*dst++ = *data++;
	}
	return;
}
