Hello!
This is my workspace to learn how to program the STM32H7 Micrcontroller.
I am using the STM32H754ZI as the platform.
As of today there are two simple projects in this repo

Project 1: Control onboard LEDs, including animatiopn and turen off at push of the button

This project helped me understand and learn how to control onboard LEDs, animate them by adjusting the time and confdigure GPIO to register the button


Project 2:Control WS2812B LEDs

This project introduced me to timer configuration and how to get the timings exactly as per the datasheet.
To drive these LEDs I used Timer 3 channel 2 and DMA set to Memory to peripheral.

I used PWM. The project is just a hello world! of driving these LEDs and if you try it out you will have LED 1 be lit Blue.

Note if you flashed this, and noticed LED is not turning on, press the reset button. 
