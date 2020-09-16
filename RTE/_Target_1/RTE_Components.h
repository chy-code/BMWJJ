
/*
 * Auto generated Run-Time-Environment Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'BMWJJ' 
 * Target:  'Target 1' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "stm32f10x.h"

/*  ARM::CMSIS:RTOS2:Keil RTX5:Library:5.5.0 */
#define RTE_CMSIS_RTOS2                 /* CMSIS-RTOS2 */
        #define RTE_CMSIS_RTOS2_RTX5            /* CMSIS-RTOS2 Keil RTX5 */
/*  Keil.ARM Compiler::Compiler:Event Recorder:DAP:1.4.0 */
#define RTE_Compiler_EventRecorder
          #define RTE_Compiler_EventRecorder_DAP
/*  Keil.ARM Compiler::Compiler:I/O:File:File System:1.2.0 */
#define RTE_Compiler_IO_File            /* Compiler I/O: File */
          #define RTE_Compiler_IO_File_FS         /* Compiler I/O: File (File System) */
/*  Keil.MDK-Plus::File System:CORE:LFN Debug:6.11.0 */
#define RTE_FileSystem_Core             /* File System Core */
          #define RTE_FileSystem_LFN              /* File System with Long Filename support */
          #define RTE_FileSystem_Debug            /* File System Debug Version */
/*  Keil.MDK-Plus::File System:Drive:Memory Card:6.11.0 */
#define RTE_FileSystem_Drive_MC_0       /* File System Memory Card Drive 0 */

/*  Keil.MDK-Plus::USB:CORE:Release:6.13.0 */
#define RTE_USB_Core                    /* USB Core */
          #define RTE_USB_Core_Release            /* USB Core Release Version */
/*  Keil.MDK-Plus::USB:Device:6.13.0 */
#define RTE_USB_Device_0                /* USB Device 0 */

/*  Keil.MDK-Plus::USB:Device:HID:6.13.0 */
#define RTE_USB_Device_HID_0            /* USB Device HID instance 0 */

/*  Keil::CMSIS Driver:SPI:2.2 */
#define RTE_Drivers_SPI1                /* Driver SPI1 */
        #define RTE_Drivers_SPI2                /* Driver SPI2 */
        #define RTE_Drivers_SPI3                /* Driver SPI3 */
/*  Keil::CMSIS Driver:USB Device:2.1 */
#define RTE_Drivers_USBD0               /* Driver USBD0 */
/*  Keil::Device:StdPeriph Drivers:ADC:3.5.0 */
#define RTE_DEVICE_STDPERIPH_ADC
/*  Keil::Device:StdPeriph Drivers:DMA:3.5.0 */
#define RTE_DEVICE_STDPERIPH_DMA
/*  Keil::Device:StdPeriph Drivers:EXTI:3.5.0 */
#define RTE_DEVICE_STDPERIPH_EXTI
/*  Keil::Device:StdPeriph Drivers:Flash:3.5.0 */
#define RTE_DEVICE_STDPERIPH_FLASH
/*  Keil::Device:StdPeriph Drivers:Framework:3.5.1 */
#define RTE_DEVICE_STDPERIPH_FRAMEWORK
/*  Keil::Device:StdPeriph Drivers:GPIO:3.5.0 */
#define RTE_DEVICE_STDPERIPH_GPIO
/*  Keil::Device:StdPeriph Drivers:IWDG:3.5.0 */
#define RTE_DEVICE_STDPERIPH_IWDG
/*  Keil::Device:StdPeriph Drivers:PWR:3.5.0 */
#define RTE_DEVICE_STDPERIPH_PWR
/*  Keil::Device:StdPeriph Drivers:RCC:3.5.0 */
#define RTE_DEVICE_STDPERIPH_RCC
/*  Keil::Device:StdPeriph Drivers:TIM:3.5.0 */
#define RTE_DEVICE_STDPERIPH_TIM
/*  Keil::Device:StdPeriph Drivers:USART:3.5.0 */
#define RTE_DEVICE_STDPERIPH_USART


#endif /* RTE_COMPONENTS_H */
