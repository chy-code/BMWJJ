
#include <stdio.h>
#include "USART_Util.h"


static void pGPIO_Init(
    GPIO_TypeDef *GPIOx,
    uint16_t TXPin,
    uint16_t RXPin)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = TXPin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = RXPin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOx, &GPIO_InitStruct);
}


static void pDMA_Init(
    USART_TypeDef *USARTx,
    DMA_Channel_TypeDef* DMAy_Channelx,
    bool PeripheralToMemory,
    void * MemoryBaseAddr,
    uint16_t BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(uint8_t*)MemoryBaseAddr;
    DMA_InitStructure.DMA_DIR = PeripheralToMemory ? DMA_DIR_PeripheralSRC : DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = BufferSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMAy_Channelx, &DMA_InitStructure);
}


static DMA_Channel_TypeDef* pGetDMAChannel_TX(USART_TypeDef *USARTx)
{
    assert_param(IS_USART_ALL_PERIPH(USARTx));

    if (USARTx == USART1)
        return DMA1_Channel4;
    if (USARTx == USART2)
        return DMA1_Channel7;
    if (USARTx == USART3)
        return DMA1_Channel2;
    if (USARTx == UART4)
        return DMA2_Channel5;

    return NULL;
}


static DMA_Channel_TypeDef* pGetDMAChannel_RX(USART_TypeDef *USARTx)
{
    assert_param(IS_USART_ALL_PERIPH(USARTx));

    if (USARTx == USART1)
        return DMA1_Channel5;
    if (USARTx == USART2)
        return DMA1_Channel6;
    if (USARTx == USART3)
        return DMA1_Channel3;
    if (USARTx == UART4)
        return DMA2_Channel3;

    return NULL;
}



void MY_USART_Init(USART_TypeDef *USARTx, uint32_t BaudRate, uint16_t Parity)
{
    if (USARTx == USART1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                               | RCC_APB2Periph_USART1, ENABLE);
        pGPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Pin_10);
    }
    else if (USARTx == USART2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        pGPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Pin_3);
    }
    else if (USARTx == USART3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        pGPIO_Init(GPIOB, GPIO_Pin_10, GPIO_Pin_11);
    }
    else if (USARTx == UART4)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        pGPIO_Init(GPIOC, GPIO_Pin_10, GPIO_Pin_11);
    }

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = BaudRate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = Parity;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStruct);
}


void MY_USART_EnableDMA(USART_TypeDef *USARTx)
{
    if (USARTx == USART1
            || USARTx == USART2
            || USARTx == USART3)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    else
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
}


void MY_USART_EnableIRQ(USART_TypeDef *USARTx)
{
    if (USARTx == USART1)
        NVIC_EnableIRQ(USART1_IRQn);
    else if (USARTx == USART2)
        NVIC_EnableIRQ(USART2_IRQn);
    else if (USARTx == USART3)
        NVIC_EnableIRQ(USART3_IRQn);
    else if (USARTx == UART4)
        NVIC_EnableIRQ(UART4_IRQn);
}


void MY_USART_EnableRXNE(USART_TypeDef *USARTx)
{
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
}


void MY_USART_Transmit_DMA(USART_TypeDef *USARTx,
                           const void *Buf,
                           uint16_t BytesToTransfer)
{
    DMA_Channel_TypeDef* DMAy_Channelx;
    DMAy_Channelx = pGetDMAChannel_TX(USARTx);

    pDMA_Init(USARTx, DMAy_Channelx, false, (void*)Buf, BytesToTransfer);

    USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);

    DMA_Cmd(DMAy_Channelx, DISABLE);
    DMAy_Channelx->CNDTR = BytesToTransfer;
    DMA_Cmd(DMAy_Channelx, ENABLE);
}



bool MY_USART_IsTransmitComplete(USART_TypeDef *USARTx)
{
    DMA_Channel_TypeDef* DMAy_Channelx;
    DMAy_Channelx = pGetDMAChannel_TX(USARTx);

    if (DMA_GetCurrDataCounter(DMAy_Channelx))
        return false;

    return true;
}


static void pDiscard(USART_TypeDef *USARTx)
{
    FlagStatus stat = USART_GetFlagStatus(USARTx, USART_FLAG_RXNE);
    if (stat == SET)
    {
        USART_ReceiveData(USARTx);
    }

    stat = USART_GetFlagStatus(USARTx, USART_FLAG_ORE);
    if (stat == SET)
    {
        USART_ClearFlag(USARTx, USART_FLAG_ORE);
        USART_ReceiveData(USARTx);
    }
}


void MY_USART_Receive_DMA(USART_TypeDef *USARTx,
                          void *Buf,
                          uint16_t BytesToTransfer)
{
    DMA_Channel_TypeDef* DMAy_Channelx;
    DMAy_Channelx = pGetDMAChannel_RX(USARTx);

    pDiscard(USARTx);

    pDMA_Init(USARTx, DMAy_Channelx, true, Buf, BytesToTransfer);
    USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

    DMA_Cmd(DMAy_Channelx, DISABLE);
    DMAy_Channelx->CNDTR = BytesToTransfer;
    DMA_Cmd(DMAy_Channelx, ENABLE);
}


bool MY_USART_IsReceiveComplete(USART_TypeDef *USARTx)
{
    DMA_Channel_TypeDef* DMAy_Channelx;
    DMAy_Channelx = pGetDMAChannel_RX(USARTx);

    if (DMA_GetCurrDataCounter(DMAy_Channelx))
        return false;

    return true;
}


void MY_USART_Transmit(USART_TypeDef *USARTx,
                       const uint8_t *Buf,
                       uint16_t BytesToTransfer)
{
    for (uint16_t i = 0; i < BytesToTransfer; i++)
    {
        USART_ClearFlag(USARTx, USART_FLAG_TC);
        USART_SendData(USARTx, Buf[i]);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}
