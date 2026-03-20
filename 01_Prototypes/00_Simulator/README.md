
# Living Color

***

## Server Simulator and Visualizer 

A simple python-based visualizer and connection testbed. This can be used in conjuction with LivingColorFXSender to test and debug connection between the desktop and the controller (simulated).

To use, run `simulator_server.py` to setup the server, then run LivingColorFXSender. The data being displayed in both apps should be the same.

***

## Dependencies
- [Python](https://www.python.org) >=v3.10 (tested on 3.10 and 3.12)
- Pygame - [Home](https://pygame.org) - [PyPI](https://pypi.org/project/pygame/) (tested on v2.6.1)
***

## Project Structure

- `simulator_server.py` - The main file, with no dependencies beyond what's in the import list and `requirements.txt`

***

## Customizing and Extending

There probably isn't a ton here that's worth customizing or extending from a development standpoint. From a usage standpoint, you can adjust the following variables:

- `WIN_W, WIN_H` - Display window width and height
- `UDP_PORT, UDP_ADDR` - This can be changed based on your network setup. Supports common address aliases (localhost, 0.0.0.0, etc)
- `FPS` - This probably doesn't need to change, but it should probably be in the neighborhood of the client FPS
- `NUM_LEDS_X, NUM_LEDS_Y` - Dimensions for the LED grid

***

## Future Work
- Read LED layouts from an external config file that's shared between the server and the client

***

## FAQ

Questions, comments, diatribes? Send me an email (seth@goindiebro.com)!
