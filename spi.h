#ifndef _SPI_H
#define _SPI_H
#include "stm32f10x.h"
extern DMA_InitTypeDef dma_config;
#define BUFFER_RX 135
extern unsigned char spi_rx[];
void SPI_config_slave(void);
void SPI_config_DMA(void);
#endif