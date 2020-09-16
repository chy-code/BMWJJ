#ifndef _USART_UTIL_H
#define _USART_UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"


void MY_USART_Init(USART_TypeDef *USARTx, 
					uint32_t BaudRate,
					uint16_t Parity);

void MY_USART_EnableDMA(USART_TypeDef *USARTx);
void MY_USART_EnableIRQ(USART_TypeDef *USARTx);
void MY_USART_EnableRXNE(USART_TypeDef *USARTx);

void MY_USART_Transmit_DMA(USART_TypeDef *USARTx, 
						const void *Buf,
						uint16_t BytesToTransfer);

bool MY_USART_IsTransmitComplete(USART_TypeDef *USARTx);

void MY_USART_Receive_DMA(USART_TypeDef *USARTx, 
						void *Buf, 
						uint16_t BytesToTransfer);

bool MY_USART_IsReceiveComplete(USART_TypeDef *USARTx);

void MY_USART_Transmit(USART_TypeDef *USARTx, 
						const uint8_t *Buf, 
						uint16_t BytesToTransfer);

#endif
