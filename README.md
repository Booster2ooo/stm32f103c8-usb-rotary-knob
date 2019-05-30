# Introduction
This project is meant to be a USB knob capable of sending user-defined keyboard combination based on rotary encoder(s) inputs.

This is my first baremetal (and C) project. I choose to work with the STM32F103C8T6 because, even if it's a bit outdate, it's still a very powerfull device at low cost.

Special thanks to sabu for his guidance, patience and help, this project wouldn't exist without him.


# Project roadmap
## [x] Handle hardware inputs 
The first step of this project is to connect the rotary encoder(s) to the STM32F103C8T6 and to intercept the inputs.

There are two kinds of inputs to manage on the rotary encoder, the switch (RSW) and the rotation (RCLK/RDT).

The switch is handled with a GPIO EXTI, triggered both when rising and falling. The interupt starts a timer (TIM2) used to virtually debounce the switch state (virtual low pass filter).

The rotation is handled with two GPIO EXTI trigged both when rising and falling, one for the clock, the other for data. The state/direction is stabilized using an algo I found reading this [article](https://www.best-microcontroller-projects.com/rotary-encoder.html).

## [x] FreeRTOS (threading)
To avoid blocking the main thread, I decided to use the FreeRTOS capabilities of the STM32F103C8T6.

The default task awaits for mails sent from the rotary encoder and behave accordingly.

## [ ] Composite USB: VCP + HID
CubeMX allows to easilly configure the device to USB Virtual Com Port mode but this is not enought.

The device should be recognized as both a VCP and an HID (keyboard) so it could send keys combination and be programmed using COM.

## [ ] Programming key combos
The VCP interface should allow the user to program the key combination that are sent when the rotary is pressed, released, turned clockwiser or counter-clockwise.