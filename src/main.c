#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx.h>
#include <usart.h>
#include <gpio.h>
#include <thread.h>

#include <my_stdio.h>

#define PENDSV_BIT (1 << 28)

//extern void mleko();
//extern void PendSV_Handler();
/*
 * ------------------ TODO ------------------ 
 *
 * - Properly configure baudrate
 * - Read about clocks and how to get current frequency
 *
 */


static volatile uint32_t msTicks; // default initializes to 0

static void sysTickInit(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;	
	msTicks = 0;
	SysTick->LOAD = 0x3e7fU;
	SysTick->CTRL = 1 | (1 << 1) | (1 << 2);
}


void SysTick_Handler()
{
	msTicks++;
	__disable_irq();
	thread_schedule();
	__enable_irq();
}


void delay_ms(uint32_t ms)
{
	uint32_t now = msTicks;

	while((msTicks - now) < ms)
	{ }
}

/* Configures USART and GPIO pins required for USART */
static void USART_Init()
{
	const uint32_t rx_pin = 3, tx_pin = 2;

	/* Enable clocks for peripherals */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	/* GPIO Configuration */
	GPIO_SetModer(GPIOA, GPIO_Mode_AF, tx_pin);
	GPIO_SetModer(GPIOA, GPIO_Mode_AF, rx_pin);

	GPIO_SetPUPDR(GPIOA, GPIO_Pupdr_PU, tx_pin);
	GPIO_SetPUPDR(GPIOA, GPIO_Pupdr_PU, rx_pin);

	GPIO_SetAF(GPIOA, GPIO_AF_USART, tx_pin);
	GPIO_SetAF(GPIOA, GPIO_AF_USART, rx_pin);


	
	/* USART Configuration */
	USART_Disable(USART2);
	USART_SetBaudRate(USART2, 115200);
	USART_SetWordLength(USART2, USART_Word_8b);
	USART_DisableParityCheck(USART2);
	USART_SetStopBits(USART2, USART_Stop_1);
	USART_SetMode(USART2, USART_Transmitter | USART_Receiver);
	USART_SetHarwareControl(USART2, USART_NoHwControl);
	USART_Enable(USART2);
}

void test_thread(void)
{
	__disable_irq();
	_printf("Starting thread id: %d\r\n", thread_get_current_id());
	__enable_irq();

	while(1) {
		__disable_irq();
		_printf("Thread id: %d\r\n", thread_get_current_id());
		__enable_irq();
	}
}


int main()
{
	__disable_irq();

	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_EnableIRQ(PendSV_IRQn);
	NVIC_SetPriority(PendSV_IRQn, 0xff); /* Lowest pririty */

	thread_module_setup();

	thread_create(test_thread, 512);
	thread_create(test_thread, 512);
	thread_create(test_thread, 512);
	thread_create(test_thread, 512);

	/* Initialization of sysTick */
	sysTickInit();

	/* Initialization of USART (printf) */
	USART_Init();


	_printf("\r\nBoard initialized correctly!\r\n");

	start_schedule();

	/* Never reached */

	__enable_irq();

	while(1) {
		__WFI();
	}
	
}
