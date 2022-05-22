#ifndef MILEDISPLAY_H_
#define MILEDISPLAY_H_

// *******************************************************
//
// miledisplay.h
//
// Display the information to OLED
//  Tiva processor.
//
// Author: Heng Yin
// Last modified:  19.5.2022
//
// *******************************************************

#include <stdint.h>
#include <stdint.h>
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "acceleration.h"
#include <math.h>
#include "milebuttons.h"
#include "circBufT.h"

//******************************************************************
/* Display [number of steps] interface.
 *          display current steps
 *              push UP button could change UINT [number of steps || percentage of Goal]
 *              push LEFT button change interface [Distance]
 *              push RIGHT button change interface [Setting Goal]
 */
void displayStepNum(int stepcounter, bool step_state, int seted_goal);

// *******************************************************
/* Display [Distance] interface.
 *          display current distance
 *              push UP button could change UINT [miles || kilometers]
 *
 *              push LEFT button change interface [Setting Goal]
 *              push RIGHT button change interface [Step]
 */
void displayDistance(int stepcounter,bool dist_state);

// *******************************************************
/* Display [Setting Goal] interface.
 *          display & adjust current Goal value by rotate the petentialmeter
 *          display setted Goal value.
 *
 *          push DOWN button confirm the Goal value,
            replace the setted Goal value by current Goal value && turn interface to [Step]
 *
 *          push LEFT button change interface [Step]
 *          push RIGHT button change interface [RIGHT]
 */
void settingGoal(int goal,int setted_goal);

// *******************************************************
//According to UIstate_t->state to display interface
void UI_Interface(int stepCounter,UIstate_t State, int goal,int seted_goal);

/*********************************************************
 *
   calculate mean value
// meanValue: return average value of this BUFFER
// stable the value */
int meanValue(int32_t sum, circBuf_t* adder, int8_t size);

#endif /*MILEDISPLAY_H_*/
