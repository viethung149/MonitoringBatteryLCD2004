#include"timer.h"
void init_timer_interrupt_MS(uint32_t tim)//1ms-1000ms
{
	TIM_TimeBaseInitTypeDef TIM;
	RCC_ClocksTypeDef ClksFreq;
	NVIC_InitTypeDef nvic;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM.TIM_CounterMode=TIM_CounterMode_Up;
	float f=1/(float)tim *1000;	
	uint32_t period=7200/f-1;
	TIM.TIM_Period=period;
	TIM.TIM_Prescaler=9999;
	TIM_TimeBaseInit(TIM_channel_MS,&TIM);
	TIM_ClearFlag(TIM_channel_MS,TIM_FLAG_Update);
	TIM_ITConfig(TIM_channel_MS,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM_channel_MS,ENABLE);
	//config interrupt
	nvic.NVIC_IRQChannel=TIM4_IRQn;
	nvic.NVIC_IRQChannelCmd=ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority=1;
	nvic.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&nvic);
}
void init_timer_interrupt_US(uint32_t tim)//1us-100us
{
	TIM_TimeBaseInitTypeDef TIM;
	RCC_ClocksTypeDef ClksFreq;
	NVIC_InitTypeDef nvic;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM.TIM_CounterMode=TIM_CounterMode_Up;	
	uint32_t period=72*tim-1;
	TIM.TIM_Period=period;
	TIM.TIM_Prescaler=0;
	TIM_TimeBaseInit(TIM_channel_button,&TIM);
	TIM_ClearFlag(TIM_channel_button,TIM_FLAG_Update);
	TIM_ITConfig(TIM_channel_button,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM_channel_button,ENABLE);
	//config interrupt
	nvic.NVIC_IRQChannel=TIM2_IRQn;
	nvic.NVIC_IRQChannelCmd=ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority=1;
	nvic.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&nvic);
}
void init_timer_read_button (uint32_t t){
	TIM_TimeBaseInitTypeDef TIM;
	RCC_ClocksTypeDef ClksFreq;
	NVIC_InitTypeDef nvic;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM.TIM_CounterMode=TIM_CounterMode_Up;
	float f=1/(float)t*1000;	
	uint32_t period=7200/f-1;
	TIM.TIM_Period=period;
	TIM.TIM_Prescaler=9999;
	TIM_TimeBaseInit(TIM_channel_button,&TIM);
	TIM_ClearFlag(TIM_channel_button,TIM_FLAG_Update);
	TIM_ITConfig(TIM_channel_button,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM_channel_button,ENABLE);
	//config interrupt
	nvic.NVIC_IRQChannel=TIM2_IRQn;
	nvic.NVIC_IRQChannelCmd=ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority=1;
	nvic.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&nvic);
}