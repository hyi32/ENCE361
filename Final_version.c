//*****************************************************************************
//
// PFM_F.c   --Personal Fitness Monitor (Final edition)
//
// Author: Heng Yin
// Last modified:	19/05/2020
//
//*****************************************************************************
// Based on the     'ADCdemo1.c'     from     P. J. Bones UCECE
//                  'uartDemo.c'     from     P. J. Bones UCECE
//                  'readAcc.c'      from     C. P. Moore
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_i2c.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "acc.h"
#include "i2c_driver.h"
#include "acceleration.h"
#include "miledisplay.h"
#include "buttons4.h"
#include "milebuttons.h"
#include "DEBUG_ONLY.h"

//*****************************************************************************
// Constants / Magic Number
//*****************************************************************************
#define BUF_SIZE 5
#define SAMPLE_RATE_HZ 10
#define SW1 GPIO_PIN_7  
// switch 1 address, from Orbit BoosterPack Reference Manual
// Digilent Inc. Digilent.com (2013)  page 4 
#define H_norm 130      // the borderline of norm value
#define L_norm 95
#define Default_Goal 1000;

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t r_inBuffer;        // Setting Goal_value BUFFER
static circBuf_t x_inBuffer;        // x axis BUFFER
static circBuf_t y_inBuffer;        // y axis BUFFER
static circBuf_t z_inBuffer;        // z axis BUFFER

static UIstate_t StepState;
//   state  => UI interface :             [step] [distance] [setting goal]
//   step   => toggle: step_unit          [number of step / percentage of goal]
//   dist   => toggle: distance_unit      [mile / kilometer]
static int Goal = Default_Goal;           // default goal as 1000 steps
static int stepCounter;                   // The number of steps

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3);  // A3 is potentiometer (Setting Goal_value)
    updateButtons();                    // Check button state (make sure response faster than user)
    Send_DEBUG_Period();                //[DEBUG]
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
{
	uint32_t ulValue;           // container for carry the potential resistance value

	//
	// Get the single sample from ADC0.  ADC_BASE is defined in
	// inc/hw_memmap.h
	ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
	//
	// Place it in the circular buffer (advancing write index)
	writeCircBuf (&r_inBuffer, ulValue);
	//

	// Clean up, clearing the interrupt
	ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
// Initialization functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock(void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

//*******************************************************************
void
initSysTick(void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / 100);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void 
initADC (void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  
    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);    
                             
    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);
  
    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
  
    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void
initDisplay(void)
{
    // initialize the Orbit OLED display
    OLEDInitialise();
}

//*******************************************************************
//  HENG YIN Original Code
//*******************************************************************

//*****************************************************************************
/*InitSwitch1: set up SW1
 Address: [A7]
 Current: 4 mA & initialize state is LOW.
 the logic state is depend on Hardware.
 Turn on => Test mode*/
//*****************************************************************************
void
initSwitch1(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPadConfigSet(GPIO_PORTA_BASE, SW1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(GPIO_PORTA_BASE, SW1, GPIO_DIR_MODE_HW);
}

//*****************************************************************************
//Initial Led: set up LED(F2)_[OUTPUT]  => Blue||Green LED
//Blue  : long press
//Green : short press
//Current : 4 mA & initialize state is LOW.
//*****************************************************************************
void
initLED(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPadConfigSet(GPIO_PORTF_BASE, BLUE_LED | GREEN_LED, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(GPIO_PORTF_BASE, BLUE_LED | GREEN_LED, GPIO_DIR_MODE_OUT);
    GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED | GREEN_LED, 0x00);
}

//******************* CounterStep Adder Method -- Norm ******************************************************
// visualize norm data, compare TIVA board movement with norm data to determine range of norm.
//According norm value, increment number of steps
void
NormSA(vector3_t meanAcc)
{
//flag, whether enable step counter increment
    static bool normFlag = true;
//use stabled accelerometer data [Unit: 0.01g]
    meanAcc.x = meanAcc.x * 100 / 256;
    meanAcc.y = meanAcc.y * 100 / 256;
    meanAcc.z = meanAcc.z * 100 / 256;
//Calculate the norm of current position
    int16_t norm = sqrt(meanAcc.x * meanAcc.x + meanAcc.y * meanAcc.y + meanAcc.z * meanAcc.z);
//Detect the monitor moving: number of step counter +1 & disable step counter increment.
    if (norm >= H_norm && !normFlag)
    {
        normFlag = true;                          // locked step counter
        stepCounter++;
//Enable step counter increment
    } else if (norm < L_norm) normFlag = false;   // unlock step counter

//  transfer norm to computer
    Transfer_DATA(norm); //[DEBUG] transfer data pop on screen  
}

/********************************************************
 * main
 ********************************************************/
int
main(void)
{
    int* stepC_pt = NULL;
    stepC_pt = &stepCounter;             // recall number of step

    UIstate_t* ss_pt = NULL;
    ss_pt = &StepState;                  // display Menu

    int* Goal_pt = NULL;
    Goal_pt = &Goal;                     // the Goal of step (setted)

    uint8_t Test_Mode_Enable;            // check SW1 to determine Test | Normal Mode

    int meanR = 0;                       // stabled potentialmeter value (setting Goal_value)

	vector3_t acceleration_raw;          // raw acceleration data
	vector3_t meanAcc;                   // stabled acceleration data

	// Initialize
	initClock();
	initADC();
	initDisplay();
    initButtons();
    initSwitch1();
    initLED();
    initSysTick();
	initCircBuf(&x_inBuffer, BUF_SIZE);
	initCircBuf(&y_inBuffer, BUF_SIZE);
	initCircBuf(&z_inBuffer, BUF_SIZE);
	initCircBuf(&r_inBuffer, BUF_SIZE);
	initAccl();
    initialiseUSB_UART();                // [DEBUG] initialize USB_UART

// Enable interrupts to the processor.
    IntMasterEnable();

    SysCtlDelay(200);  // wait for the initialization 

	while (1)
	{
// Initiation data
	    meanAcc.x = 0, meanAcc.y = 0, meanAcc.z = 0;
	    meanR = 0;

// Get 3 axis data from ADC
	    acceleration_raw = getAcclData();

// Write in data to buffer
	    writeCircBuf(&x_inBuffer, acceleration_raw.x);
	    writeCircBuf(&y_inBuffer, acceleration_raw.y);
	    writeCircBuf(&z_inBuffer, acceleration_raw.z);

// Calculate mean value
	    meanAcc.x = meanValue(meanAcc.x, &x_inBuffer, BUF_SIZE);
	    meanAcc.y = meanValue(meanAcc.y, &y_inBuffer, BUF_SIZE);
	    meanAcc.z = meanValue(meanAcc.z, &z_inBuffer, BUF_SIZE);

// The potentialmeter [setting Goal_valu]
	    meanR = meanValue(meanR, &r_inBuffer, BUF_SIZE) / 40 * 100; // change Goal value range & unit as "100 steps"

// Detect whether active Test_Mode
	    Test_Mode_Enable = GPIOPinRead(GPIO_PORTA_BASE, SW1);

// The monitor response by buttons state changed.
	    CheckEachButton(ss_pt, Test_Mode_Enable, stepC_pt, Goal_pt, meanR);

//*** CounterStep Method -- Norm ***
	    NormSA(meanAcc);

// Display the calculated parameter to OLED
	    UI_Interface(stepCounter, StepState, meanR, Goal);
	}
}
