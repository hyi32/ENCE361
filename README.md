Base 'ADCdemo1' 'readAcc' develop Personal Fitness Monitor.  
The user's steps will be increments by ADXL345 digital accelerometer sensor data.

# Author:  
      Heng Yin

<!-- include lib: 'buttons4.c', 'buttons4.h', 'circBufT.c', 'circBufT.h'... -->

# This device has 3 interfaces [Steps | Distance | Goal Setting]   
(Default switch 1 in DOWN position)
- The steps interface shows the number of steps the user has done and how close they are to their steps goal.
- The Distance interface shows how far the user has traveled.
- The Goal setting interface can be used to changed the users step goal.

- The interfaces can be cycled though by pushing the left and right buttons on the tiva board
> Push Left button =>   Cycle though interface options  
> Push Right button  => Cycle in reverse though interface options

## [Steps]

* UP button  toggle Unit **[number of steps | percentage]**  
 >- Number of steps  
    shows total steps the user has walked/ran.  
 >- Percentage:  
    shows the percentage of completed steps towards goal.      
    when goal was achieved, display **Goal Achieved!!!**

* DOWN button (long press)    
>- Resets the step and distance counts  
>- LED unit flashes **Blue** to tell user that reset was sucessful 



## [Distance]
* UP button  toggle Unit **[mile | kilometer]**  
* DOWN button (long press)    
>- Resets the step and distance counts  
>- LED unit flashes **Blue** to tell the user that reset was sucessful 


 ## [Goal Setting]

* **Default**        ===>       shows new step goal setting & current daily step goal.

    - Rotate potentiometer => adjust new step goal value.
      - => increments the new step goal by units of 100 steps
      - => Rotate clockwise to increase step goal.
      - => Rotate anti-clockwise to Decrease step goal.

    - (short) Push Down Button
      - => sets a new step goal as daliy goal.
      - => LED unit flashes **Green** to tell the user that a new goal has been set
      - => interface returns to the [Steps] interface

#  This device also has a TEST mode (switch 1 in UP position)
  - Left and Right buttons are unaffected by TEST MODE and work normaly
    > Push Left button   => Cycle though interface options  
    > Push Right button  => Cycle in reverse though interface options
### TEST MODE:
- Push UP Button  
-- Increase step count by increments of 100  
-- distance + 0.09 km
- Push DOWN Button  
-- Decrease step count by increments of 500 steps.  
-- ditance - 0.45 km

### Debug Only
> Enable "DEBUG" part code  
> send the **norm** to computer. To determined the step counter triggle range.  
norm = sqrt(x^2 + y^2 + z^2);

