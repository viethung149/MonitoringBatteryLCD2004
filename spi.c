#include"spi.h"

void SPI_config_slave(void){
	GPIO_InitTypeDef gpio_config;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	gpio_config.GPIO_Pin = GPIO_Pin_14;
	gpio_config.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_config);
	
	gpio_config.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	gpio_config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_config);
	
	gpio_config.GPIO_Pin = GPIO_Pin_12;
	gpio_config.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_config);
	
	SPI_InitTypeDef spi_config;
	spi_config.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi_config.SPI_Mode = SPI_Mode_Slave;
	spi_config.SPI_DataSize = SPI_DataSize_8b;
	spi_config.SPI_CPOL =SPI_CPOL_Low;
	spi_config.SPI_CPHA = SPI_CPHA_1Edge;
	spi_config.SPI_NSS = SPI_NSS_Hard;
	spi_config.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	spi_config.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_config.SPI_CRCPolynomial = 0x0007;
	SPI_Init(SPI2,&spi_config);
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
	SPI_Cmd(SPI2,ENABLE);
	/* Configure and enable SPI_SLAVE interrupt --------------------------------*/
	
	      NVIC_InitTypeDef NVIC_InitStructure;
				NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
	
}
void SPI_config_DMA(){
	// channel 4 SPI 2 RX
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel4);
	DMA_ClearFlag(DMA1_IT_TC4);
	DMA_InitTypeDef dma_config;
	dma_config.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;;
	dma_config.DMA_MemoryBaseAddr = (uint32_t)&spi_rx;
	dma_config.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma_config.DMA_BufferSize = BUFFER_RX ;
	dma_config.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_config.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_config.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;
	dma_config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;
	dma_config.DMA_Mode = DMA_Mode_Circular;
	dma_config.DMA_Priority = DMA_Priority_High;
	dma_config.DMA_M2M = DMA_M2M_Disable;
	// FIFO store data before it was extraced to the destination
	DMA_Init(DMA1_Channel4,&dma_config);
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Rx,ENABLE);
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel4,ENABLE);
  NVIC_InitTypeDef it_config;
	it_config.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	it_config.NVIC_IRQChannelCmd = ENABLE;
	it_config.NVIC_IRQChannelPreemptionPriority = 0 ;
	it_config.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&it_config);
	
	
}
