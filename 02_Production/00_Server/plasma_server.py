'''plasma_server.py
This file is part of the Living Color project (https://github.com/SethGIB/ProjectLivingColor)

Copyright (C) 2026 Seth Gibson

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

PORTIONS OF THIS CODE ARE BASED ON THE FOLLOWING:
- Cinder (https://libcinder.org/)
- ASIO (https://think-async.com/Asio/)
- OpenCV (https://opencv.org/)
- RealSense SDK (https://realsenseai.com/)

AND ARE USED IN COMPLIANCE WITH THEIR RESPECTIVE LICENSES. SEE THEIR WEBSITES FOR MORE INFORMATION.'''

import socket
import time
import plasma
from plasma import plasma2040
from ezwifi import connect

UDP_PORT = 50051
UDP_ADDR = '0.0.0.0'
NUM_LEDS = 24 #216
BUFFER_SIZE = NUM_LEDS * 3
FPS = 30
TICK = 1.0 / float(FPS)

def wifi_failed(message=""):
    print(f'Wifi connection failed! {message}')
    # Could add fallback logic here (e.g. flashing red LEDs)

def wifi_message(wifi, message):
    print(message)

try:
    print("Connecting to Wi-Fi...")
    connect(failed=wifi_failed, info=wifi_message, warning=wifi_message, error=wifi_message)
except ValueError as e:
    wifi_failed(e)

# Set up a non-blocking UDP socket to receive RGB data for the LEDs
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_ADDR, UDP_PORT))
sock.setblocking(False) # Prevents the loop from freezing while waiting for data
print(f"Listening on UDP port {UDP_PORT}")

led_strip = plasma.WS2812(NUM_LEDS, color_order=plasma.COLOR_ORDER_RGB)
led_strip.start(FPS)

while True:
    try:
        # Pass the max buffer size (we expect RGB values for our LEDs)
        data, addr = sock.recvfrom(BUFFER_SIZE)
        
        # Calculate how many LEDs we received data for
        obj_count = len(data) // 3
        num_leds = min(obj_count, NUM_LEDS)
        
        # Apply the colors efficiently without creating intermediate lists
        for i in range(num_leds):
            r = data[i*3]
            g = data[i*3 + 1]
            b = data[i*3 + 2]
            led_strip.set_rgb(i, r, g, b)
            
    except OSError:
        # In MicroPython, non-blocking sockets throw OSError when no data is ready
        pass

    time.sleep(TICK)