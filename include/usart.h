#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

#include <stm32f4xx.h>

/* USART Defines */


/* Control Register 1 */
#define USART_CR1_WL 12U		/* USART Word Length bit */

#define USART_CR1_PSB 9U			/* USART Pariry select bit */

#define USART_CR1_MB 2U			/* USART Mode bits [2:3] */

#define USART_CR2_STOPB 12U		/* USART Stop bits [12:13] */


/* USART Enums */
typedef enum {
	USART_Word_8b = 0,
	USART_Word_9b = 1,
} USART_WordLength;

typedef enum {
	USART_Par_EVEN = 0,
	USART_Par_ODD = 1,
} USART_Parity;

typedef enum {
	USART_Stop_1 = 0,
	USART_Stop_0_5 = 1,
	USART_Stop_2 = 2,
} USART_StopBits;

typedef enum {
	USART_None = 0,
	USART_Transmitter = 1,
	USART_Receiver = 2,
} USART_Mode;

typedef enum {
	USART_NoHwControl,
} USART_HarwareControl; 

/* USART Control functions */
void USART_Enable(USART_TypeDef *USARTx);
void USART_Disable(USART_TypeDef *USARTx);
void USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t br);
void USART_SetWordLength(USART_TypeDef *USARTx, USART_WordLength wl);
void USART_EnableParityCheck(USART_TypeDef *USARTx);
void USART_DisableParityCheck(USART_TypeDef *USARTx);
void USART_SetParity(USART_TypeDef *USARTx, USART_Parity par);
void USART_SetStopBits(USART_TypeDef *USARTx, USART_StopBits sb);
void USART_SetMode(USART_TypeDef *USARTx, uint32_t mode);
void USART_SetHarwareControl(USART_TypeDef *USARTx, USART_HarwareControl hc);
void USART_Transmit(USART_TypeDef *USARTx, uint8_t data);

#endif /* __USART_H__ */
