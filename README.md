# Introduction
This project is meant to be a USB knob capable of sending user-defined keyboard combination based on rotary encoder(s) inputs.

This is my first baremetal (and C) project. I choose to work with the STM32F103C8T6 because, even if it's a bit outdate, it's still a very powerfull device at low cost.

Special thanks to sabu (email: sabu (at) serveur (dot) io) for his guidance, patience and help, this project wouldn't exist without him.


# Project roadmap
## [x] Handle hardware inputs 
The first step of this project is to connect the rotary encoder(s) to the STM32F103C8T6 and to intercept the inputs.

There are two kinds of inputs to manage on the rotary encoder, the switch (RSW) and the rotation (RCLK/RDT).

The switch is handled with a GPIO EXTI, triggered both when rising and falling. The interupt starts a timer (TIM2) used to virtually debounce the switch state (virtual low pass filter).

The rotation is handled with two GPIO EXTI trigged both when rising and falling, one for the clock, the other for data. The state/direction is stabilized using an algo I found reading this [article](https://www.best-microcontroller-projects.com/rotary-encoder.html).

## [x] FreeRTOS (threading)
To avoid blocking the main thread, I decided to use the FreeRTOS capabilities of the STM32F103C8T6.

The default task awaits for mails sent from the rotary encoder and behave accordingly.

## [x] Composite USB: VCP + HID
CubeMX allows to easilly configure the device to USB Virtual Com Port mode but this is not enought.

The device should be recognized as both a VCP and an HID (keyboard) so it could send keys combination and be programmed using COM.

## [x] Programming key combos
The VCP interface should allow the user to program the key combination that are sent when the rotary is pressed, released, turned clockwiser or counter-clockwise.

## [ ] Save/load config to/from flash/eeprom
Once the rotary is programmed, the flash memory should be written for the configuration to be persistant when the device in unpluged/pluged again.

# Links
Here are a few link to projects and articles that helped me during the process (aside from ST manuals):

- https://satoshinm.github.io/blog/171227_stm32hid_pill_duck_scriptable_usb_hid_device_using_an_stm32_blue_pill_from_mouse_jigglers_to_rubber_duckies.html
- https://notes.iopush.net/stm32-custom-usb-hid-step-by-step-2/
- https://gitlab.com/piotrva/stm32-composite-hid-cdc
- https://github.com/robotsrulz/STM32-COMPOSITE
- https://habr.com/ru/post/335018/ (translated to EN thanks to G Translate)
- https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
- https://www.stefanjones.ca/blog/arduino-leonardo-remote-multimedia-keys/