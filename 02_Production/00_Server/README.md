
# Living Color

***

## LED Server 

***

## Dependencies
- [MicroPython](https://micropython.org/)
    - The current version uses a [Pimoroni firmware for the Plasma 2350 W](https://github.com/pimoroni/plasma/releases/tag/v1.1.0)
- LED module for your chosen stack (e.g. NeoPixels, plasma, etc). These tend to come standard with whatever Python you're using for your MCU, whether it's Micro or Circuit

***

## Project Structure

- `plasma_server.py` - The main file. This version is specific to the Pimoroni MicroPython stack, but the core code can be used with other MCUs.
- To use this one as-is, I recommend opening a connection to your Plasma 2350 W via [Thonny](https://thonny.org) and copying this code into `main.py`

***

## Customizing and Extending

The two main things I could imagine wanting to customize are the socket protocol (use TCP vs UDP, etc) and setting the LEDs. 

The code for the UDP setup starts at `line 28`, and all the LED relevant code happens on the `led_strip` object, so change those per your needs and it should just work. I've only tested this on a Pimoroni Plasma 2350 W, but any MCU that can accept a UDP connection and unpack a bytebuffer should just work with minor changes.

***

## Future Work
- Implement UDP broadcast pattern

***

## FAQ

Questions, comments, diatribes? Send me an email (seth@goindiebro.com)!
