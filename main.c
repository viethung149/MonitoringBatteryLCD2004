#include "stm32f10x.h"
#include "timer.h"
#include "i2c_lcd.h"
#include "NgatNgoai.h"
#include "spi.h"
#include "string.h"
#include "stdio.h"
#include "uart.h"
/*
*  PA3 receive data
*  PC14 handshark
*  PA15 button
*  PB7  SDA
*  PB6  SCL
*/

#define NUMBER_BATTERY 8
#define NUMBER_FLOAT_DATA 32
#define TIME_WAIT_READ 5000
#define TIME_READ_BUTTON 5
#define MAX_READ 40 // 5ms*40 = 200ms (read in 200ms) 
#define SENSITIVE_BUTTON 3 // if in 200ms have 10 times button is 1 mean this button was press
DMA_InitTypeDef dma_config;
uint32_t Time_ST=0;	 // bien delay bang system tick timer	
int number_cell = 0;
int number_package =0;
unsigned char spi_rx [BUFFER_RX];
unsigned char crc_check = 0x00;
float voltage[NUMBER_BATTERY];
float temperature[NUMBER_BATTERY];
float float_buffer[NUMBER_FLOAT_DATA];
int cnt_test =0;
BitAction Status_voltage[NUMBER_BATTERY] = {Bit_RESET};
BitAction Status_temperature[NUMBER_BATTERY] = {Bit_RESET};
BitAction Status_temp1[NUMBER_BATTERY] ={Bit_RESET};
BitAction Status_temp2[NUMBER_BATTERY] ={Bit_RESET};
BitAction Flag_read_cell = Bit_SET;
BitAction Flag_read_package = Bit_RESET;
BitAction update = Bit_RESET;
BitAction proccess_buffer = Bit_RESET;
int counter_read_spi =0;
int counter_dma_work =0;
int number_byte_index =0;
int counter_it =0;
int counter_overall = 0;
int counter_button =0;
union floatToByte
{
	float variableFloat;
	char varialbeByte[4];
};
int FAIL = 0;
int SUCCESSFUL =0;
/*
* Config pin PC13 to test, and pin PC14 for handshark
*/

void config_pin_handsharkTest()
{
  GPIO_InitTypeDef GPIO_initStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_initStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_initStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;
	GPIO_initStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOC,&GPIO_initStructure);  
}
/*
* Config pin PA15 is input mode for reading the status of button
*/
void config_button(){
	GPIO_InitTypeDef GPIO_initStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  GPIO_initStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_initStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_initStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_initStructure);  
}
void Delay_ms(uint16_t time)
{
uint32_t time_n=time*12000;
	while(time_n!=0){time_n--;}
}
void Delay_msST(uint16_t nTime)
{
uint32_t time_n=nTime*12000;
	while(time_n!=0)
	{
		time_n--;
	}
}
int main(void)
	{
	SysTick_Config(SystemCoreClock/1000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	config_pin_handsharkTest();               // config pin handshark pin PC14 and pin test PC13
  config_button();  	                      // config button PA15
  I2C_init_LCD();              							// config I2c Protocol for LCD
	lcd_init();                  						// config LCD in mode 4 line
	//SPI_config_slave();                       // config SPI2 in slave mode for reading data from stm32f407
	init_timer_interrupt_MS(TIME_WAIT_READ);  // config TIMER4 for waiting to read stm32f407
	init_timer_read_button(TIME_READ_BUTTON); // config TIMER2 for reading the status of button
	lcd_send_string("viet hung");
	UART_Configuration();
	while(1){
		// if button is pressed render again LCD  
		if(update == Bit_SET)              
		{
			lcd_infor_cell(voltage,temperature,number_cell,number_package);
			update = Bit_RESET;
		}
		// if reading correct data frame from stm32 get the value and show it in the LCD
		if(proccess_buffer == Bit_SET){
			
			unsigned char crc = 0x00;
			for(int i =0 ;i< 133;i++){crc += spi_rx[i];}
			if(spi_rx[0] == 'I' && crc == spi_rx[133]){
				// from 1 to 128 is data
				int index_float_buffer =0;
				for(int i =1 ;i< 129;)
				{
					union floatToByte temp;
					temp.varialbeByte[0] = spi_rx[i++];
					temp.varialbeByte[1] = spi_rx[i++];
					temp.varialbeByte[2] = spi_rx[i++];
					temp.varialbeByte[3] = spi_rx[i++];
					float_buffer[index_float_buffer++]= temp.variableFloat;
				}
				// from 129 to 132 is status
				// status temperature byte 129
				for(int i =0;i<8;i++){
					if((spi_rx[129] & 0x80) == 0x80){
					  Status_temperature[i] = Bit_SET;
					}
					else{
						Status_temperature[i] = Bit_RESET;
					}
				}
				// status voltage byte 130
				for(int i =0;i<8;i++){
					if((spi_rx[130] & 0x80) == 0x80){
					  Status_voltage[i] = Bit_SET;
					}
					else{
						Status_voltage[i] = Bit_RESET;
					}
				}
				// two byte that free of using
				// status temp1 byte 131
				for(int i =0;i<8;i++){
					if((spi_rx[131] & 0x80) == 0x80){
					  Status_temp1[i] = Bit_SET;
					}
					else{
						Status_temp1[i] = Bit_RESET;
					}
				}
				// status temp2 byte 132
				for(int i =0;i<8;i++){
					if((spi_rx[132] & 0x80) == 0x80){
					  Status_temp2[i] = Bit_SET;
					}
					else{
						Status_temp2[i] = Bit_RESET;
					}
				}
				// can read another frame SPI DMA
				for(int i =0;i<8;i++){
					temperature[i] = float_buffer[i];
					voltage[i] = float_buffer[i+16];
				}
				for(int i =0;i<NUMBER_FLOAT_DATA;i++)
					{
						float_buffer[i]=0;
					}
				/*for(int i =0;i<135;i++)
					{
						spi_rx[i]=0;
					}*/
				counter_read_spi++;
				if(number_cell !=0 || number_package != 0)
				lcd_infor_cell(voltage,temperature,number_cell,number_package);
			}
	    else{
				//				
				
			}
			for(int i =0;i<NUMBER_FLOAT_DATA;i++)
					{
						float_buffer[i]=0;
					}
			for(int i =0;i<135;i++)
				{
					spi_rx[i]=0;
				}
			proccess_buffer=Bit_RESET;
		}
	}
}

/*
* Handler USART2 receive the data from stm32f407
* subpriority:  0
* pre: 0
*/
void USART2_IRQHandler(void){
	if (USART_GetFlagStatus(USART2, USART_IT_RXNE) != RESET){
		counter_it++;if(counter_it>1350) counter_it=0;
			spi_rx[number_byte_index]=USART_ReceiveData(USART2);
		  if(spi_rx [0] != 'I'){
				number_byte_index = 0;
				goto end;
			}
		  if(number_byte_index <133){
				crc_check += spi_rx[number_byte_index];
				number_byte_index++;
			}
			else if(number_byte_index == 133 && crc_check == spi_rx[133]){
				  SUCCESSFUL++;
				  crc_check = 0x00;
					number_byte_index =0;
					proccess_buffer = Bit_SET;
			}
			else{
					number_byte_index = 0;
				  FAIL++;
			}
		  end:
			USART_ClearFlag(USART2, USART_IT_RXNE);
	}
}
/*
* Handler to send handshark signal to smt32f407 to syn data
* subpriority:  1
* pre: 0
*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		// send handshark
		GPIO_WriteBit(GPIOC,GPIO_Pin_14,Bit_SET);
    GPIO_WriteBit(GPIOC,GPIO_Pin_14,Bit_RESET);		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
/*
* Handler TIMMER 2 for reading button
* subpriority:  1
* pre: 0
*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		// send handshark
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)(1^GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)));	
	  counter_overall++;
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) == Bit_SET) counter_button++;
			if(counter_overall == MAX_READ){
				if(counter_button > SENSITIVE_BUTTON){
					if(Flag_read_cell == Bit_SET){
				     number_cell++;
						if(number_cell == 9) {
							number_cell =0;
							Flag_read_cell = Bit_RESET;
							Flag_read_package = Bit_SET;
						}
			}
			if(Flag_read_package == Bit_SET){
				number_package ++;
				if(number_package == 3) {
					number_package =0;
					Flag_read_package = Bit_RESET;
					Flag_read_cell = Bit_SET;
					number_cell++;
				}		
			}
			update = Bit_SET;
				}
				counter_overall= 0;
				counter_button = 0;
			}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}