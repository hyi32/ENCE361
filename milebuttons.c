// *******************************************************
//
// milebuttons.c
//
// Support for a set of FOUR specific buttons on the Tiva/Orbit.
//
// The Buttons function:
//
// Normal Mode:
// Up       ->     change the UNIT (number of step, distance)
// Down     ->     (short press)  confirm the goal
//          ->     (long press)   RESET the step & distance
// Left     ->     change the UI [Step, Distance, Setting Goal]
// Right    ->     Reverse Left button
//
// --------------------------------------------------------
// TEST Mode:
// Up       ->     Step + 100   &  Distance + 0.9 km
// Down     ->     Step - 500   &  Distance - 4.5 km
//
//--------------------------------------------------------
//
// Heng Yin
// Last modified:  19.5.2022
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>
#include "buttons4.h"
#include "milebuttons.h"
#include "acceleration.h"
#include "driverlib/gpio.h"

// *******************************************************
// Global variables
// *******************************************************
static bool flag_DownButton;
static int8_t reset_counter;

// *******************************************************
// CheckEachButton: Polling each button.
//parameters:
//    ss_pt                  UI_state pointer
//    ss_pt.state   =>       UI menu [Number of steps || Goal setting || Total distance]
//    ss_pt.step    =>       Step Unit [number/ percentage of Goal]
//    ss_pt.dist    =>       Distance Unit [mile / kilometer]
//
//
//    Test_Mode_Enable:       ON  =>  Normal Mode
//                            OFF =>  TEST Mode    (SW1 on the Orbit board)
//
//    stepC_pt:           Connect to Number of Steps
//
//    Goal_pt:            Connect to Goal
//
//    Rvalue:             Potentialmeter Value (select Goal value)
//
void CheckEachButton(UIstate_t* ss_pt, uint8_t Test_Mode_Enable, int* stepC_pt, int* Goal_pt, int Rvalue)
{
//    Interface options [Steps | Distance | Goal Setting]
//    Push left button  => Cycle though interface options
//    Push right button => Cycle in reverse though interface options
    checkLeftButton(checkButton(LEFT), ss_pt);
    checkRightButton(checkButton(RIGHT), ss_pt);

//    Normal mode:  UP button    => change unit [step/distance]
//                  DOWN button  => (short press) in [Goal setting]     => setted goal turn to [number of steps] interface.
//                               => (long press)  in [step || distance] => reset number of steps & distances
//    Test mode:    UP button    => [number of steps] + 100 && [distances] + 0.09 km
//                  DOWN button  => [number of steps] - 500 && [distances] - 0.45 km
    checkUpButton (checkButton(UP), Test_Mode_Enable, stepC_pt, ss_pt);
    checkDownButton(checkButton(DOWN),Test_Mode_Enable, stepC_pt, Goal_pt,Rvalue, ss_pt);

//    Detect is long press or short press
    isLongPress(stepC_pt, Test_Mode_Enable);
}

// *******************************************************
// checkLeftButton: Push left button  => Cycle though interface options

// ss_pt.state == 0 [Step]
//                1 [Distance]
//                2 [Setting Goal]
void checkLeftButton(uint8_t butState, UIstate_t* ss_pt)
{
    if (butState == PUSHED)
    {
        if (ss_pt->state == 3) ss_pt->state = 0;
        ss_pt->state++;
        blink_GLed();
    }
}

// *******************************************************
// checkRightButton: Push right button => Cycle in reverse though interface options
void checkRightButton(uint8_t butState, UIstate_t* ss_pt)
{
    if (butState == PUSHED)
    {
        if (ss_pt->state == 0 ) ss_pt->state = 3;
        ss_pt->state--;
        blink_GLed();
    }
}

// *******************************************************
// checkUpButton:
//         Normal Mode:     Toggle the UNIT (number of steps, distances)
//                          UI_state.step == 0 [number of steps]
//                                        == 1 [percentage of Goal]
//
//                          UI_state.dist == 0 [miles]
//                                        == 1 [kilometers]
//
//         TEST Mode:       Step + 100   &  Distance + 0.09 km
void checkUpButton (uint8_t butState, uint8_t Test_Mode_Enable, int* stepC_pt, UIstate_t* ss_pt )
{
    if(butState == PUSHED)
    {
        if (Test_Mode_Enable)              // Test Mode
        {
            *stepC_pt = *stepC_pt + 100;            // Step + 100 & Distance + 0.09 km
            blink_GLed();                           // flash Green LED; tell user the operate successes.
        } else {                           // Normal Mode
//                                                  // switch unit
            switch(ss_pt->state)
            {
            case 1:                             // [DISTANCE]
                ss_pt->dist = !ss_pt->dist;         // toggle unit of distances [miles || kilometers]
                blink_GLed();                       // flash Green LED; tell user the operate successes.
                break;
            case 2:                             // [Goal setting] NO operate
                break;
            default:                            // [STEP]
                ss_pt->step = !ss_pt->step;         // toggle unit of steps [numbers || percentage]
                blink_GLed();                       // flash Green LED; tell user the operate successes.
                break;
            }
        }
    }
}

// *******************************************************
// checkDownButton:
//         Normal Mode:
//                 (short presss) in [Goal Setting] => confirm the Goal value && turn to [STEP]
//                 long press in [STEP || DISTANCE] => Reset Steps & Distances.
//
//         TEST Mode:     Decrease the UNIT (number of step, distance)
//                        Step - 500   &  Distance - 0.45 km
void checkDownButton(uint8_t butState, uint8_t Test_Mode_Enable, int* stepC_pt, int* Goal_pt, int Rvalue, UIstate_t* ss_pt)
{
    if (butState == PUSHED)
    {
        flag_DownButton = true;                          // The down button been pushed.
        if (Test_Mode_Enable)                            // TEST Mode
        {
            *stepC_pt = *stepC_pt - 500;                        //number of steps - 500
            if(*stepC_pt < 0) *stepC_pt = 0;                    // if number of steps less than 0, set it as 0.
            blink_GLed();                                // flash Green LED; tell user the operate successes.
        } else {                                         // Normal Mode
            if (ss_pt->state == 2)                       // UI State is [Setting Goal]
                {
                    *Goal_pt = Rvalue;                    // Confirm Goal , when UI State is [Setting Goal]
                    ss_pt->state = 0;                    // back to [Step] Mode.
                    blink_GLed();                         // flash Green LED; tell user the operate successes.
                }
        }

    }

    if (butState == RELEASED ){
        flag_DownButton = false;                         // The down button released.  It is short press.
        reset_counter = 0;                               // reset counter back to 0, No affect next pushing.
    }
}

// *******************************************************
// isLongPress: long press down button  => reset [Step & Distance]
void isLongPress(int* stepC_pt, uint8_t Test_Mode_Enable)
{

    if (flag_DownButton && Test_Mode_Enable == 0)                                // Push down button
    {
        reset_counter++;                                // recall time of push down button

        if (reset_counter > LongTime)                   // Holding time more than Threshold confirm it is long press
        {
            reset_counter = 0;                          // RESET reset_counter

                *stepC_pt = 0;                          // RESET number of step & distance
                blink_BLed();                           // flash BLUE LED; tell user the long press operate successes.
        }
    }

}

// Flash BLUE LED
void blink_BLed(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED,BLUE_LED);    // turn on led
    SysCtlDelay(SysCtlClockGet()/12);                    // continue light up Blue led
    GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED,0x00);        // turn off led
}

// FLASH GREEN LED
void blink_GLed(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED,GREEN_LED);  // turn on led
    SysCtlDelay(SysCtlClockGet()/36);                    // continue light up Green led
    GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED,0x00);       // turn off led
}

