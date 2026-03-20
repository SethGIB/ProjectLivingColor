
# Living Color

***

This is the complete project source for Living Color, first presented at Illuminate Salt Lake, March 6-7 2026. You are free to use, re-use, edit, modify, distribute, re-distribute, commercialize, tutorialize, whatever you want really, but keep in mind that this code is released under the GPL license, which means that any changes or contributions **MUST BE OPEN SOURCED.** Please keep that in mind before proceeding.

## Project Structure
- **`00_Previz`** - Look/Vizdev files
- **`01_Prototypes`** - Demos, Tests, R&D
    - **`00_Simulator`** - Python testbed for server/client comms and visualization
    - **`01_Plasma`** - Lookdev/R&D for the color cycling effects
        - **`00_ShaderED_Projects`**
        - **`01_Cinder_Projects`**
- **`02_Production`** - Start here if you want to build your own
    - **`00_Server`** - Micropython code for driving LEDs from data over Wifi/UDP
    - **`01_Client`** - Desktop client code for generating and sending colors to the MCU
- **`03_Fabrication`** - Files and tools for building the enclosure
    - **`00_glassgen`** - Processing tool for generating voronoi patterns for the "stained glass" effect
    - **`01_LED_Enclosure`** - Blender, STL, and 3MF files for the enclosure

Specific information can be found in each section's `README.md`. Questions, comments, diatribes? Send me an email (**seth@goindiebro.com**)!

***

## FAQ
**Q: Why the GPL License?? GPLs are SOOOOO RESTRICTIVE!!!**
- A: Yeah, that's kinda the point. I don't mind if you want to make money off of this and in fact I encourage it, the "licensing fee" I'm charging is that you help continue to improve the project so everyone can benefit from everyone else's unique experience and knowledge. The main reason I'm releasing this code is LEARNING. 
- A: Also, as listed in the LICENSE.txt and the file headers, certain parts of this code that reference external libraries, SDKs, and frameworks are used under their respective licenses.
- A: Also also, there's nothing stopping you from just copying and pasting or transcribing the bits you need. I'm never gonna know.

**Q: Why did you implement "x" this way or why didn't you implement (some pattern that probably makes more sense and is better architected & optimized)?**
- A: Because I had three weeks to finish this and about 4 other installation pieces for the festival, so I kinda just hip-fired a lot of this. That said, I am indeed a humble puck and would LOVE suggestions, PRs, and Issues!

**Q: (A really specific question about a really specific part of the project)?**
- A: Well, first check the above question, then check the README for the relevant section. Then, if it passes those gates, I'd be happy to! Send me an email (**seth@goindiebro.com**) or log an Issue please!

**Q: Can you develop a custom version of this for us?**
- A: Yup, I am 100% available for consulting, commissions, and custom development. Send me an email (**seth@goindiebro.com**) and let's make some art happen!

**Q: Hey, we'd like to invite you to (art show, gallery, or festival)!**
- A: I'm definitely open to that! Send me an email (**seth@goindiebro.com**) and let's talk terms and timelines!

**Q: (Anything else, hopefully related to this or my other projects)?**
- A: Send me an email (**seth@goindiebro.com**)!

***

## Greetz
- **Andrew Bell of [Rare Volume](https://rarevolume.com) and the Cinder community** - Y'all the real MVPs. Thanks for keeping the Cinder gears turning after all these years.
- T**he fine folks at [RealSense](https://realsenseai.com)** - Congrats on the spinout, Keep on Keepin' On!
- **All the Creative Techies out there that are still doin' the do in the face of a wildly changin landscape** - Keep it up, the world needs humans-in-the-loop now more than ever.