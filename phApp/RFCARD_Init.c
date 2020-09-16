
#include <stdint.h>
#include "cmsis_os2.h"
#include "stm32f10x.h"
#include "RFCARD_App_Rc531.h"
#include "phhalHw_Rc523.h"


#define RC523_PD_PORT		GPIOD
#define RC523_PD_PIN		GPIO_Pin_6

#define RC523_SCK_PORT		GPIOD
#define RC523_SCK_PIN		GPIO_Pin_3

#define RC523_MISO_PORT		GPIOD
#define RC523_MISO_PIN		GPIO_Pin_4

#define RC523_MOSI_PORT		GPIOD
#define RC523_MOSI_PIN		GPIO_Pin_2

#define RC523_CS_PORT		GPIOD
#define RC523_CS_PIN		GPIO_Pin_5


static __forceinline
void Set_SCK_High(void)
{
    GPIO_SetBits(RC523_SCK_PORT, RC523_SCK_PIN);
}

static __forceinline
void Set_SCK_Low(void)
{
    GPIO_ResetBits(RC523_SCK_PORT, RC523_SCK_PIN);
}

static __forceinline
void Set_MOSI_High(void)
{
    GPIO_SetBits(RC523_MOSI_PORT, RC523_MOSI_PIN);
}

static __forceinline
void Set_MOSI_Low(void)
{
     GPIO_ResetBits(RC523_MOSI_PORT, RC523_MOSI_PIN);
}

static __forceinline
void Set_CS_High(void)
{
    GPIO_SetBits(RC523_CS_PORT, RC523_CS_PIN);
}

static __forceinline
void Set_CS_Low(void)
{
    GPIO_ResetBits(RC523_CS_PORT, RC523_CS_PIN);
}


static __forceinline
void Set_PD_High(void)
{
	GPIO_SetBits(RC523_PD_PORT, RC523_PD_PIN);
}


static __forceinline
void Set_PD_Low(void)
{
	GPIO_ResetBits(RC523_PD_PORT, RC523_PD_PIN);
}

static __forceinline
uint8_t Read_MISO(void)
{
    return GPIO_ReadInputDataBit(RC523_MISO_PORT, RC523_MISO_PIN);
}


static uint8_t SPI_SendData(uint8_t b);
static uint8_t SPI_ReceiveData(void);
static void Delay(void);


void RC523_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	// PD Pin
	GPIO_InitStructure.GPIO_Pin = RC523_PD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RC523_PD_PORT, &GPIO_InitStructure);
	
    // SPI MOSI
    GPIO_InitStructure.GPIO_Pin = RC523_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(RC523_MOSI_PORT, &GPIO_InitStructure);

    // SPI CLK
    GPIO_InitStructure.GPIO_Pin = RC523_SCK_PIN;
    GPIO_Init(RC523_SCK_PORT, &GPIO_InitStructure);

    // SPI CS
    GPIO_InitStructure.GPIO_Pin = RC523_CS_PIN;
    GPIO_Init(RC523_CS_PORT, &GPIO_InitStructure);
	
    // SPI MISO
    GPIO_InitStructure.GPIO_Pin = RC523_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(RC523_MISO_PORT, &GPIO_InitStructure);
	
	Set_CS_High();
	Set_PD_Low();
}


void RC523_Identify(void)
{
	uint8_t bReg;
	
	Set_PD_High();
	Delay();
	
	Set_PD_Low();
	Delay();;
	
	Set_PD_High();
	Delay();
	
	phhalHw_Rc523_ReadRegister(0, 0x17, &bReg);
	if (bReg!=0x84)
	{
		bInterfaceRFIC = 0xff;
	} 
	else
	{
	  bInterfaceRFIC = 1;
	}
}


uint8_t RC523_ReadByte(const uint8_t addr)
{
	uint8_t b;
	
	Set_CS_Low();
	Delay();
	
	SPI_SendData(addr);
	b = SPI_ReceiveData();
	
	Set_CS_High();
	Delay();
	
	return b;
}


void RC523_WriteByte(const uint8_t addr, uint8_t data)
{
	Set_CS_Low();
	Delay();
	
	SPI_SendData(addr);
	SPI_SendData(data);
	
	Set_CS_High();
	Delay();
}


uint8_t SPI_SendData(uint8_t b)
{
	uint8_t temp = 0;
	
	for (int i = 0; i < 8; i++)
	{
		Set_SCK_Low();
		
		if (b & 0x80)
			Set_MOSI_High();
		else
			Set_MOSI_Low();
		
		b <<= 1;
		Delay();
		Set_SCK_High();
		temp <<= 1;
		temp |= Read_MISO();
		
		Delay();
	}
	
	return temp;
}


uint8_t SPI_ReceiveData(void)
{
	return SPI_SendData(0x00);
}


void Delay(void)
{
	for (int i = 0; i < 100; i++);
}
