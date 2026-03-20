
# Living Color

***

## ShaderFBOTest

***

## Dependencies
- Cinder >= 0.9.3 - [Download](https://libcinder.org/download) - [Source](https://github.com/cinder/Cinder)
- Set a `CINDER_PATH` environment variable that points to the root of your Cinder directory
***

## Project Structure

### Shaders

- `plasma.vert`
	- A simple pass-through shader that forwards vertex positions and texture coordinates to the frag shaders
- `plasma.frag`
	- A basic plasma 

### Source Files

- `ShaderFBOTestApp.cpp`
	- Contains the definitions and implementations for the application. Loads shaders and textures, creates all the necessary objects, renders the shaders to FBOs, and displays the results.

### Other Files

The textures used to generate the plasma effects are located in the `assets/textures` folder. These include various noise textures and a gradient texture that is used to map the noise values to colors.

***

## FAQ

Questions, comments, diatribes? Send me an email (seth@goindiebro.com)!