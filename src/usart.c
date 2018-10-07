#include <usart.h>

/* USART Functions */
void USART_Enable(USART_TypeDef *USARTx)
{
	USARTx->CR1 |= USART_CR1_UE;
}

void USART_Disable(USART_TypeDef *USARTx)
{
	USARTx->CR1 &= ~USART_CR1_UE;
}

void USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t br)
{
	(void)br;
	USARTx->BRR = 0x8b;
}


void USART_SetWordLength(USART_TypeDef *USARTx, USART_WordLength wl)
{
	USARTx->CR1 &= ~(1 << USART_CR1_WL);
	USARTx->CR1 |= (wl << USART_CR1_WL);
}

void USART_EnableParityCheck(USART_TypeDef *USARTx)
{
	USARTx->CR1 |= USART_CR1_PCE;
}

void USART_DisableParityCheck(USART_TypeDef *USARTx)
{
	USARTx->CR1 &= ~USART_CR1_PCE;
}

void USART_SetParity(USART_TypeDef *USARTx, USART_Parity par)
{
	USARTx->CR1 &= ~(1 << USART_CR1_PSB);
	USARTx->CR1 |= (par << USART_CR1_PSB);
}

void USART_SetStopBits(USART_TypeDef *USARTx, USART_StopBits sb)
{
	USARTx->CR2 &= ~(3 << USART_CR2_STOPB);
	USARTx->CR2 |= (sb << USART_CR2_STOPB);
}

void USART_SetMode(USART_TypeDef *USARTx, uint32_t mode)
{
	USARTx->CR1 &= ~(3 << USART_CR1_MB);
	USARTx->CR1 |= (mode << USART_CR1_MB);
}

void USART_SetHarwareControl(USART_TypeDef *USARTx, USART_HarwareControl hc)
{
	(void)USARTx;
	(void)hc;
}

void USART_Transmit(USART_TypeDef *USARTx, uint8_t data)
{
	USARTx->DR = data;
}
