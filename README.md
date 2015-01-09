# FRC_Status_LEDs
Code to be used with a RioDuino and WS2812 LEDs to provide visual feedback to the driver of an FRC robot
<hr>
This program is designed to take single characters from a serial input
and change the color of a section of an LED strip based on the input. An optional LED
"show" of various kinds can be started upon arduino boot or upon a specific character input. (Intended for use
when the robot is otherwise idle.) The "show" will end when any character is passed into the serial input.
<p />

Use of a WS2812 (aka. NeoPixel) compatible LED string is required (presence of the Adafruit Neopixel library assumed)
<p />

The number of sections in the LED display is definable, HOWEVER:
the number of LEDs in the string is assumed to be evenly divisible by the number of sections specified
(code behavior is undefined if this requirement is not met)
<p />

Having a single white LED between sections is optional. When enabled, relative direction of the string will be
indicated by the single white LED lit at the far end of status sections. This may be useful for disambiguation
as mentioned below. This does reduce the size of each section by one LED.
<p />

For usability reasons, care should be taken to avoid ambiguous status color schemes like red-white-red-white-red. It would become more difficult
to tell which end was which and therefore which section of the display corresponded to which "function" of the robot in such a case.
<p />

Be mindful that this code is intended to divide a SINGLE string of WS2812 LEDs into sections. This is probably not what you want if you're working with a
single string that wraps to different sides of the robot, or where you've got multiple strings daisy chained together all over the robot. The working
assumption in this code is that if multiple status displays are desired (for instance on multiple sides of the robot), they should all be divided
in the same way and be showing the same thing. This means that the data input for all of the strings must start at the ARDUINO, not at another string.
You can build a splitter cable to facilitate this. One partial motivation for this decision was the ability to keep some of the status
displays working should one of them take damage during match play. Only one output is provided, which also means that all strings are assumed to have the
same number of pixels.
<p />

Details about the implementation, controls, and options may be found in the comments integrated in the "Settings" section at the top of the code. Please read there for more information.
