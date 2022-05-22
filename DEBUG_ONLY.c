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
#include "DEBUG_ONLY.h"

char statusStr[MAX_STR_LEN + 1];
volatile uint8_t slowTick = false;

//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************
void
initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void
UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

//**********************************************************************
// set transfer data information interval
void Send_DEBUG_Period(void)
{
    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

    if (++tickCount >= ticksPerSlow)
    {                                              // Signal a slow tick
        tickCount = 0;
        slowTick = true;
    }
}

//**********************************************************************
//transfer data by UART_USB
void Transfer_DATA(int16_t data)
{
    if (slowTick)
        {
            slowTick = false;
            // Form and send a status message to the console
            usprintf (statusStr, "Norm = %3d  | \r\n", data ); //
            UARTSend (statusStr);
        }
}

