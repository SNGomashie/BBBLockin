# Enhanced Capture (eCAP) Module
In this example we will learn how to use the Enhanced capture (eCAP) module with the PRUs. This module is capable of measuring rising and falling edge events and can use these to extract the following information:

- Sample rate
- Speed measurements of rotating machinery
- Elapsed time measurements betwen position sensor pulses
- Period and duty cycle measurements of pulse train signals
- Decoding current or voltage amplitude derived from duty cycle encoded current/voltage sensors

We will be using this module to capture the period(frequency) of the function generator. Based on this period we can generate a sinewave with the same frequency using a PLL. More information about the eCAP module can be found in __*TI am335x TRM Chapter 15.3 Enhanced Capture (eCAP) Module*__

## eCAP usage
Since we are only interested in the period of our SYNC signal we can use the Time Difference (Delta) Operation Rising Edge Trigger example from the TI am335x TRM. This will be explained using the image below.

The CAPx pin is our input pin. We will connect our SYNC signal to this pin. This is pin P9_42 on the BeagleBone Black. CTR[0-31] is our counter. This counter counts at 200MHz so each value is 5ns. In time difference operation the counter will start counting until the next capture event. it will save this value in the on of the CAP registers. We have configured out eCAP module as such that it will only use one CAP register since we have no need for old period values. We only need the current period. A overflow of the CTR register can only happen when the reference frequency is too low. This would mean a period of 20 seconds since the count at 200MHZ and have a 32bit counter register. As you can see in the image, this configuration returns the period.

<img src="https://i.imgur.com/pWT98bB.png" alt="drawing"/>
