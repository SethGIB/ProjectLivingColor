
# Living Color

***

## LivingColorFXSender 

`LivingColorFXSender` is where all the heavy lifting happens. This application is responsible for:
- Reading the camera frames and post-processing them to prepare them for the computer vision magic
- Performing the computer vision magic to determine how to color the LEDs
- Generating the underlying color map that gets sampled based on the contours from the computer vision
- Bundling up all the colors and sending the bytes to the receiver running on the Plasma 2350 W
	- Note that a Plasma 2350 W or even a Raspberry Pi of any sort isn't stricly required. As long as your MCU can spin up a UDP port and receive & unpack a byte buffer (or even better, just forward the bytes as-is down the wire to the LEDs), you're good to go

Currently only tested on windows with Realsense cameras, but should be x-plat friendly with a little bit of work, as all the dependcies are also x-plat. I'll be adding Raspberry Pi support myself, please feel free to submit a PR for other platforms!

***

## Dependencies
- Cinder >= 0.9.3 - [Download](https://libcinder.org/download) - [Source](https://github.com/cinder/Cinder)
- Cinder-OpenCV3 - [Source](https://github.com/cinder/Cinder-OpenCV3)
- librealsense - [Download](https://github.com/realsenseai/librealsense/releases) - [Source](https://github.com/realsenseai/librealsense)
	- I recommend using the latest version from source and building static libraries
***

## Project Structure

### Header Files

- `LivingColorFXSender.h`
	- Contains the definitions for the main app and the implementations of the LED class, `FXLed`, and the connection manager class, `FXSender`
- `Plasma.h`
	- Contains the definitions for the color cycling effects that are used to generate the color maps for the LEDs (yes, it's a demoscene reference)

### Shaders

- `passthru.vert`
	- Exactly what it soundslike, just forwards vertex positions and texture coordinates to the frag shaders
- `plasma_x.frag`
	- Different variations of the plasma effect, but at the core, they all do the same thing, i.e., blend different noises and lookup a color from a gradient. More on this in the documentation below

### Source Files

- `LivingColorFXSenderApp.cpp`
	- Contains the implementation of the main client application. This is where the camera frames are read, processed, and where the final LED colors are generated and sent to the receiver
- `Plasma.cpp`
	- Contains the implementation of the plasma effects, which are used to generate the color maps for the LEDs. The plasma effects are implemented as GLSL shaders, and this file contains the code to set up the shaders and pass in the necessary uniforms

### Other Files

The textures used to generate the plasma effects are located in the `assets/textures` folder. These include various noise textures and a gradient texture that is used to map the noise values to colors.

***

## Customizing and Extending

Some general notes on how to work with different parts of the code for those who might be interested in building a custom version or just tweaking the existing project. I tried to cover most of the stuff I think you'd want to customize, but if there's a specific workflow or process that I haven't addressed here that you're curious about, don't hesitate to get in touch (**seth@goindiebro.com**).

### Implementing A Custom Effect

Custom effects can be implemented in a few different ways since the LEDs don't actually care about the effect itself, just the colors they get. The simplest effect would be to write a new plasma. In this case, all you have to do is create two textures:
- A four-channel (RGBA) texture that encodes a different noise pattern in each of the channels
- A gradient texture that the shader will use as a lookup table
- You can then just copy one of the existing plasma shaders and pass in the proper paths to your new textures when you create a new `Plasma` object in `LivingColorFXSenderApp::setupPlasma()`, or just replace the paths in one of the existing calls
- The are two other bits of secret sauce for plasma effects:
	- Each texture has it's own custom uvs (`uv_0`, `uv_1`, `uv_2`, and `uv_3`) that can be used to change the scrolling and otherwise animated effects. Try out all the crazy maths here
	- In some of the shaders, you'll notice that the layers are being blended Photoshop-style rather than just simple math. I recommend checking out [glsl-blend](https://github.com/jamieowen/glsl-blend) for blending functions, and using something like photoshop or affinity to do some pre-viz

Completely custom shaders can also be used, you'll just need to make sure that you're setting the uniforms properly, as right now the code relies on how things are done for this specific implementation of the plasma effects. The uniforms are set initially in `Plasma::init()` and managed in `Plasma::setActive()`.

It might actually be simpler to just write your own custom effect class, especially if you need to update uniforms per-frame or at some other interval from the caller, who knows, maybe I'll add some kind of a "plug-in" framework for effects at some point in the future.

I also recommend using [ShaderED](https://shadered.org/) for shader previz. Once you have something you like, you can pretty much just copy and paste the code into your Cinder project and just change some of the uniform names (`uTime` to `ciElapsedSeconds`, etc.).

### Using A Different Camera

This should actually be pretty simple. Most of the depth camera SDKs I've seen let you get a pointer to the raw data buffer (`uint8_t/uchar*`, etc) for whatever images the camera produces, and that's really all you need to make this work.

The first thing you'd want to do is replace all the code in `LivingColorFXSenderApp::setupCamera()` with the code to initialize your camera and get it streaming. You'll notice that I'm doing some specific post-processing provided by the Realsense SDK, including rotating the frame 90 degrees to portrait orientation, colorizing the depth frame just to crunch the values into 8-bit RGB space, and thresholding the depth so we don't get values that are too far away to be useful for the effect. Other SDKs provide similar functionality, so you should be able to approximate this pipe pretty close.

The second and main step is to make sure that in `LivingColorFXSenderApp::updateFrames()`, you run whatever post-processing needs to happen on the frames once you've polled the camera, then grab your pointer to the depth buffer or the processed depth frame. Line **257** in `updateFrames()` is where the handoff happens, so really you shouldn't need to change anything beyond that once you have your depth frame data. 

***

## Future Work
- I've been working on a Cinder-Realsense block for another project, I'll probably integrate that at some point
- Raspberry Pi support, which will likely involve some optimizations to get the computer vision running smoothly on the Pi and do away with the need for wifi/UDP
- Add the ability to read LED layouts from a config file that can be shared with the simulator
- Use a proper human segmentation model I can find something that doesn't murder the GPU. As of this writing (2025-03-11), I have a Femto Bolt in the mail, so I'll probably test against that first
- Integrate any cool suggestions from the community

***

## FAQ
**Q: Why are there shared pointer typedefs all over the place? They don't really even make sense.**
- A: True, these were just for practice. I may be doing more Cinderblock development in the near future (Cinder-RealSense, etc), and this is a pattern Cinder uses a lot.  I just wanted to get into the habit of writing my own.

**Q: Why did you implement "x" this way or why didn't you implement (some pattern that probably makes more sense and is better architected & optimized)?**
- A: Because I had three weeks to finish this and about 4 other installation pieces for the festival, so I kinda just hip-fired a lot of this. That said, I am indeed a humble puck and would LOVE suggestions, PRs, and Issues!

**Q: (A really specific question about a really specific part of the code)?**
- A: Well, first check the above question and the Documentation section. Then, if it passes those gates, I'd be happy to! Send me an email (**seth@goindiebro.com**) or log an Issue please!

**Q: Can I do (something, pretty much anything, with this code)?**
- A: Yep, that's why I released it. Just remember, this is released under the GPL, which means:
	- **Copyleft Principle:** The core requirement is that if you use GPL-licensed code in your own software, your code must also be licensed under the same GPL terms if you distribute it, which is often termed a "viral" clause.
	- **Source Code Availability:** If software is distributed (e.g., in products like routers or apps), the corresponding source code must be made available to the recipients.
	- **Freedom to Modify & Share:** Users can change, improve, or use parts of the software and redistribute their modified versions to the public.
	- **No Warranty:** The software is provided without any warranty, ensuring developers are not liable for issues, as typical in open-source licenses.
- Keep those things in mind and you can pretty much do whatever you want, and again, I'm always happy to answer questions (seth@goindiebro.com)

