#ifndef DEBUG_ONLY_H_
#define DEBUG_ONLY_H_

// *******************************************************
//
// DEBUG_ONLY.h
//
//        Transfer data to computer, help for develop or debug.
//        Transfer [norm] data help developer determind the range of norm.
//
// Author: Heng Yin
// Last modified:  14.5.2022
// *******************************************************

#include "circBufT.h"
#include "acceleration.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"


#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 4
#define MAX_STR_LEN 16
//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************
void initialiseUSB_UART(void);

//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void UARTSend (char *pucBuffer);

//**********************************************************************
// set transfer data information interval
void Send_DEBUG_Period(void);

//**********************************************************************
//transfer data by UART_USB
void Transfer_DATA(int16_t data);

#endif /*DEBUG_ONLY_H_*/

