#include "uart.h"
#include "type.h"
void UART_Configuration (void)
	{
		/*Khoi tao bien cau hinh*/
			GPIO_InitTypeDef			GPIO_InitStructure;
			USART_InitTypeDef			UART_InitStructure;
			NVIC_InitTypeDef			NVIC_InitStructure;

		/*Cap clock cho USART và port su dung*/
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
			/* Cau Rx mode FLOATING  */
		  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		/*Cau hinh USART*/
			UART_InitStructure.USART_BaudRate = 9600;
			UART_InitStructure.USART_WordLength = USART_WordLength_8b;
			UART_InitStructure.USART_StopBits = USART_StopBits_2;
			UART_InitStructure.USART_Parity = USART_Parity_No;
			UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			UART_InitStructure.USART_Mode = USART_Mode_Rx;
			USART_Init(USART2, &UART_InitStructure);
				
		/* Cau hinh vector ngat va muc uu tien */
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_STM32F407;	
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
			
			/* xoa co ngat nhan cho lan dau su dung*/
			USART_ClearFlag(USART2, USART_IT_RXNE);
			
			/* Cau hinh cho phep ngat nhan*/
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

			/* Cho phep UART hoat dong */
			USART_Cmd(USART2, ENABLE);
	
	}
