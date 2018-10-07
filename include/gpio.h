#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

#include <stm32f4xx.h>

typedef enum {
	GPIO_Mode_In = 0x00,
	GPIO_Mode_Out = 0x01,
	GPIO_Mode_AF = 0x02,
	GPIO_Mode_An = 0x03,
} GPIO_Mode;

typedef enum {
	GPIO_AF_USART = 0x07,
} GPIO_AlternateFunction;

typedef enum {
	GPIO_Pupdr_NoPuPD = 0x00,
	GPIO_Pupdr_PU = 0x01,
	GPIO_Pupdr_PD = 0x02,
} GPIO_Pupdr;


/* GPIO Functions */
void GPIO_ClearModer(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_SetModer(GPIO_TypeDef *GPIOx, GPIO_Mode mode, uint32_t pin);
void GPIO_ClearAF(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_SetAF(GPIO_TypeDef *GPIOx, GPIO_AlternateFunction af, uint32_t pin);
void GPIO_ClearPUPDR(GPIO_TypeDef *GPIOx, uint32_t pin);
void GPIO_SetPUPDR(GPIO_TypeDef *GPIOx, GPIO_Pupdr pupd, uint32_t pin);

#endif /* __GPIO_H__ */
