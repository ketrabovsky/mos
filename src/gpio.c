#include <gpio.h>


void GPIO_ClearModer(GPIO_TypeDef *GPIOx, uint32_t pin)
{
	GPIOx->MODER &= ~(3U << pin * 2);
}

void GPIO_SetModer(GPIO_TypeDef *GPIOx, GPIO_Mode mode, uint32_t pin)
{
	GPIO_ClearModer(GPIOx, pin);
	GPIOx->MODER |= (mode << pin * 2);
}

void GPIO_ClearAF(GPIO_TypeDef *GPIOx, uint32_t pin)
{
	GPIOx->AFR[pin >> 3U] &= ~(15U << pin * 4);
}

void GPIO_SetAF(GPIO_TypeDef *GPIOx,
					   GPIO_AlternateFunction af,
	   				   uint32_t pin)
{
	GPIO_ClearAF(GPIOx, pin);
	GPIOx->AFR[pin >> 3U] |= (af << pin * 4);
}

void GPIO_ClearPUPDR(GPIO_TypeDef *GPIOx, uint32_t pin)
{
	GPIOx->PUPDR &= ~(3U << pin * 2);
}

void GPIO_SetPUPDR(GPIO_TypeDef *GPIOx, GPIO_Pupdr pupd, uint32_t pin)
{
	GPIO_ClearPUPDR(GPIOx, pin);
	GPIOx->PUPDR |= (pupd << pin * 2);
}
