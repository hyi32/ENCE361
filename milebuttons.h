#ifndef MILEBUTTONS_H_
#define MILEBUTTONS_H_

// *******************************************************
//
// milebuttons.h
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
#include "acceleration.h"
#include "driverlib/gpio.h"

#define LongTime 40
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

typedef struct{
    uint8_t state;
    bool step;
    bool dist;
} UIstate_t;

// *******************************************************
// CheckEachButton: Polling each button.
//Input:
//    ss_pt:                  UI_state pointer
//    ss_pt.x =>      UI menu
//    ss_pt.y =>
//                when it been chaneged Display different UI
//
//    Test_Mode_Enable:       ON => Normal Mode
//                            OFF => TEST Mode    (SW1 on the Orbit board)
//
//    stepC_pt:           Connect to Number of Steps
//
//    Goal_pt:            Connect to Goal
//
//    Rvalue:             Potential meter Value (select Goal value)
//

void CheckEachButton(UIstate_t* ss_pt, uint8_t Test_Mode_Enable, int* stepC_pt, int* Goal_pt, int Rvalue);

// *******************************************************
// checkLeftButton: Push left button  => Cycle though interface options

// ss_pt.state == 0 [Step]
//                1 [Distance]
//                2 [Setting Goal]
void checkLeftButton(uint8_t butState, UIstate_t* ss_pt);

// *******************************************************
// checkRightButton:  Push right button => Cycle in reverse though interface options
void checkRightButton(uint8_t butState, UIstate_t* ss_pt);

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
void checkUpButton (uint8_t butState,uint8_t Test_Mode_Enable, int* stepC_pt, UIstate_t* ss_pt);

// *******************************************************
// checkDownButton:
//         Normal Mode:
//                 (short presss) in [Goal Setting] => confirm the Goal value && turn to [STEP]
//                 long press in [STEP || DISTANCE] => Reset Steps & Distances.
//
//         TEST Mode:     Decrease the UNIT (number of step, distance)
//                        Step - 500   &  Distance - 0.45 km
void checkDownButton(uint8_t butState, uint8_t Test_Mode_Enable, int* stepC_pt, int* Goal_pt, int Rvalue, UIstate_t* ss_pt);

// *******************************************************
// isLongPress: long press down button  => reset [Step & Distance]
void isLongPress(int* stepC_pt, uint8_t Test_Mode_Enable);

// *******************************************************
// FLASH BLUE LED
void blink_BLed(void);

// *******************************************************
// FLASH GREEN LED
void blink_GLed(void);

#endif /*MILEBUTTONS_H_*/
