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
import math
import plasma
from ezwifi import connect

UDP_PORT = 50051
DISCOVERY_PORT = 50050
UDP_ADDR = '0.0.0.0'
NUM_LEDS = 216
BUFFER_SIZE = NUM_LEDS * 3
DISCOVERY_MSG = b'DISCOVER'
REPLY_MSG = b'PLASMA_SERVER'
FPS = 30
TICK = 1.0 / float(FPS)

def wifi_failed(message=""):
    print(f'Wifi connection failed! {message}')
    # Could add fallback logic here (e.g. flashing red LEDs)

def wifi_message(wifi, message):
    print(message)

def setup_network():
    try:
        print("Connecting to Wi-Fi...")
        connect(failed=wifi_failed, info=wifi_message, warning=wifi_message, error=wifi_message)
    except ValueError as e:
        wifi_failed(e)

    _sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    _sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    _sock.bind((UDP_ADDR, UDP_PORT))
    _sock.setblocking(False) # Prevents the loop from freezing while waiting for data
    print(f"Listening on UDP port {UDP_PORT}")

    _discovery_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    _discovery_sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    _discovery_sock.bind((UDP_ADDR, DISCOVERY_PORT))
    _discovery_sock.setblocking(False)
    print(f"Listening for discovery on UDP port {DISCOVERY_PORT}")
    
    return _sock, _discovery_sock

def set_solid_color(r, g, b):
    for i in range(NUM_LEDS):
        led_strip.set_rgb(i, r, g, b)

sock, discovery_sock = setup_network()

led_strip = plasma.WS2812(NUM_LEDS, color_order=plasma.COLOR_ORDER_RGB)
led_strip.start(FPS)

# Started up but no client connection yet
set_solid_color(255, 64, 0)

last_data_time = 0
hue_offset = 0.0

while True:
    try:
        # Check for discovery packets
        try:
            disc_data, disc_addr = discovery_sock.recvfrom(32)
            if disc_data == DISCOVERY_MSG:
                print(f"Discovery request from {disc_addr}, sending reply")
                discovery_sock.sendto(REPLY_MSG, disc_addr)
        except OSError as e:
            if e.args[0] not in (11, 111):
                raise e

        # Pass the max buffer size (we expect RGB values for our LEDs)
        data, addr = sock.recvfrom(BUFFER_SIZE)
        last_data_time = time.time()
        
        # Calculate how many LEDs we received data for
        obj_count = len(data) // 3
        num_leds = min(obj_count, NUM_LEDS)
        
        # Apply the colors efficiently without creating intermediate lists
        for i in range(num_leds):
            r = data[i*3]
            g = data[i*3 + 1]
            b = data[i*3 + 2]
            led_strip.set_rgb(i, r, g, b)
            
    except OSError as e:
        # In MicroPython, non-blocking sockets throw OSError when no data is ready
        if e.args[0] not in (11, 111):
            print(f"Connection error: {e}. Reconnecting...")
            set_solid_color(255, 64, 0)
            try:
                sock.close()
                discovery_sock.close()
            except:
                pass
            time.sleep(2)
            sock, discovery_sock = setup_network()
            last_data_time = 0 # trigger idle animation when re-networked

    # Idle animation if no data from client for > 3 seconds
    if time.time() - last_data_time > 3.0:
        hue_offset += 0.01
        for i in range(NUM_LEDS):
            hue = (i / float(NUM_LEDS) + hue_offset) % 1.0
            led_strip.set_hsv(i, hue, 1.0, 0.4) # Dim rainbow

    time.sleep(TICK)
