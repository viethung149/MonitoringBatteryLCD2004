#include "NgatNgoai.h"
// cau hinh ngat ngoai va thu tu uu tien
void EXTI_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;// input tro keo xuong, khi chua co gi thi chan PA5 o muc 0
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  //mapping
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
  // clearing pending bit
	EXTI_ClearITPendingBit(EXTI_Line15);
 // cau hinh exti LINE
  EXTI_InitTypeDef EXTI_Structure;
  EXTI_Structure.EXTI_Line=EXTI_Line15;
  EXTI_Structure.EXTI_LineCmd=ENABLE;
  EXTI_Structure.EXTI_Mode=EXTI_Mode_Interrupt;
  EXTI_Structure.EXTI_Trigger=EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_Structure);
  // cau hinh uu tien ngat
  NVIC_InitTypeDef NVIC_Structure;
  NVIC_Structure.NVIC_IRQChannel=EXTI15_10_IRQn;
  NVIC_Structure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Structure.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_Structure.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&NVIC_Structure);
}
