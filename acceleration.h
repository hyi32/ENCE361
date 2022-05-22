#ifndef ACCELERATION_H_
#define ACCELERATION_H_

// *******************************************************

// *******************************************************
#include <stdint.h>
#include <stdint.h>
#include "stdlib.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "acc.h"
#include "i2c_driver.h"
// *******************************************************
// acc structure

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

void initAccl (void);
vector3_t getAcclData (void);

#endif /*ACCELERATION_H_*/
