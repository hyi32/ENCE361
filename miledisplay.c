// *******************************************************
//
// miledisplay.c
//
// Calculation the data & Display on the OLED.
//  Tiva processor.
//
// Author: Heng Yin
// Last modified:  19.5.2022
//
// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "utils/ustdlib.h"
#include <stdbool.h>
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "miledisplay.h"
#include "acceleration.h"
#include <math.h>
#include "circBufT.h"

// *******************************************************
/* Display [number of steps] interface.
 *          display current steps
 *              push UP button could change UINT [number of steps || percentage of Goal]
 *              push LEFT button change interface [Distance]
 *              push RIGHT button change interface [Setting Goal]
 */
void
displayStepNum(int stepcounter, bool step_state, int seted_goal)
{
    int value = stepcounter * 100 / seted_goal;                        // complete goal progress [unit: %]
    char string[17];                                                   // 16 characters across the display
    usnprintf (string, sizeof(string), "Step             ");
    OLEDStringDraw (string, 0, 0);

    if (step_state)
    {
        if (seted_goal == 0) {
            usnprintf (string, sizeof(string), "Set Goal First! ");   // if not set the goal, tell the user set goal first.
            OLEDStringDraw (string, 0, 2);
        } else if (value >= 100) {
            usnprintf (string, sizeof(string), "Goal Achieved!!!");  // if goal achieved, display the ACHIEVED!.
            OLEDStringDraw (string, 0, 2);
        } else {
            usnprintf (string, sizeof(string), "progress     %2d%%", value);  // if goal setted, display the percentage.
            OLEDStringDraw (string, 0, 2);
        }
    } else {
        usnprintf (string, sizeof(string), "      %4d steps", stepcounter);
        OLEDStringDraw (string, 0, 2);
    }

    usnprintf (string, sizeof(string), "                ");
    OLEDStringDraw (string, 0, 1);
    usnprintf (string, sizeof(string), "                ");
    OLEDStringDraw (string, 0, 3);
}

// *******************************************************
/* Display [Distance] interface.
 *          display current distance
 *              push UP button could change UINT [miles || kilometers]
 *
 *              push LEFT button change interface [Setting Goal]
 *              push RIGHT button change interface [Step]
 */
void
displayDistance(int stepcounter, bool dist_state)
{
    uint8_t num_k = 0;     // integer number [km]
    uint8_t num_h = 0;     // first digital number [km]
    uint8_t num_t = 0;     // second digital number [km]

    uint8_t mile = 0;      // integer number [mile]
    uint8_t mile_f = 0;    // first digital number [mile]
    uint8_t mile_s = 0;    // second digital number [mile]
                           // 0.62137119224   1 km => mile
    char string[17];       // 16 characters across the display
    usnprintf (string, sizeof(string), "Distance        ");
    OLEDStringDraw (string, 0, 0);

    num_k = stepcounter * 0.9 / 1000;                           // keep integer
    num_h = (stepcounter * 0.9 - num_k * 1000) / 100;           // keep first decimal
    num_t = (stepcounter * 0.9 - num_k * 1000 - num_h * 100) / 10;

    mile = stepcounter * 0.9/1000 * 0.6213711922;
    mile_f = (stepcounter * 0.9/1000 * 0.6213711922 - mile) * 10;
    mile_s = (stepcounter * 0.9/1000 * 0.6213711922 - mile ) * 100 - mile_f * 10 ;

    if (dist_state)                          // Km mode              keep digital 2 after the decimal point
    {
        usnprintf (string, sizeof(string), "disc    %2d.%1d%1d km", num_k, num_h, num_t);
        OLEDStringDraw (string, 0, 2);
    } else {
        usnprintf (string, sizeof(string), "disc    %2d.%1d%d mi", mile, mile_f, mile_s);
        OLEDStringDraw (string, 0, 2);
    }

    usnprintf (string, sizeof(string), "                ");
    OLEDStringDraw (string, 0, 1);
    usnprintf (string, sizeof(string), "                ");
    OLEDStringDraw (string, 0, 3);
}

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
void
settingGoal(int goal, int setted_goal)
{
    char string[17];                                                    // 16 characters across the display
    usnprintf (string, sizeof(string), "Goal Setting    ");
    OLEDStringDraw (string, 0, 0);
    usnprintf (string, sizeof(string), "          %6d", goal);          // show current potentialmeter value[goal value]
    OLEDStringDraw (string, 0, 1);

    if (setted_goal == 0) {
        usnprintf (string, sizeof(string), "Set Goal Now!   ");
        OLEDStringDraw (string, 0, 2);
        usnprintf (string, sizeof(string), "                ");
        OLEDStringDraw (string, 0, 3);
    } else {
        usnprintf (string, sizeof(string), "Daily Goal:     ");
        OLEDStringDraw (string, 0, 2);
        usnprintf (string, sizeof(string), "          %6d", setted_goal);  // display the setted_goal value.
        OLEDStringDraw (string, 0, 3);
    }
}

// *******************************************************
// According to UIstate_t->state to display interface
void
UI_Interface(int stepCounter, UIstate_t State ,int goal, int seted_goal)
{
    switch(State.state)
    {
    case 1:            // [Distance]
        displayDistance(stepCounter, State.dist);
        break;
    case 2:            // [Setting Goal]
        settingGoal(goal, seted_goal);
        break;
    default:           // [Step]
        displayStepNum(stepCounter, State.step, seted_goal);
        break;
    }
}

/*********************************************************
 *
   calculate mean value
// meanValue: return average value of this BUFFER
// stable the value */
int meanValue(int32_t sum, circBuf_t* adder, int8_t size)
{
    int i;
    for (i = 0; i< size; i++)
           sum = sum + readCircBuf(adder);
    sum = (2 * sum + size) / 2 / size;
    return sum;
}
