
# Living Color

***

## Stained Glass Generator

A quick stab at a small tool for creating customized voronoi maps that could be used as guides for fabricating stained glass patterns. I didn't actually use this in production, but I think it could be used to that end pretty reliably.

To use:
- Load the main sketch (`glassgen.pde`) into processing
- Left-click to place or drag a point
- Right-click to remove a point

***

## Dependencies
- [processing 4](https://processing.org)
- [Toxiclibs](https://github.com/postspectacular/toxiclibs)
- This could probably exist in some combination of [p5.js](https://p5js.org) and [Toxiclibs.js](https://github.com/hapticdata/toxiclibsjs) but I haven't tried that

***

## Project Structure
- `glassgen.pde` - The main sketch that allows you to place and edit points semi-interactively into a voronoi diagram
- `VoronoiMgr.pde` - Wraps toxiclibs' Voronoi and provides some useful functions for placing and updating points

***

## Customizing and Extending

This is pretty barebones, so there's a ton of work that could go into customizing and extending it. On the plus side, this is pretty barebones, so adding all kinds of new functionality should be pretty easy.

***

## Future Work

- Port this to Cinder or maybe even a game engine and turn this into a bigger tool

***

## FAQ
