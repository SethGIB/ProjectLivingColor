/*Plasma.h
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

AND ARE USED IN COMPLIANCE WITH THEIR RESPECTIVE LICENSES. SEE THEIR WEBSITES FOR MORE INFORMATION.*/	

#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class Plasma;
typedef std::shared_ptr<Plasma> PlasmaRef;

class Plasma
{
public:
	Plasma() {}
	void init(const std::string &vert, const std::string &frag, const std::string &noiseTex, const std::string &gradTex, const ivec2& fboDims);
	void setActive(bool active);
	void render(const ivec2& dims, const Area& bounds);
	Surface8u getPixels() const;
	gl::Texture2dRef getTexture() const { return mRenderTarget->getColorTexture(); }
	static PlasmaRef create() { return std::make_shared<Plasma>(); }

private:
	gl::Texture2dRef mPlasmaTex;
	gl::Texture2dRef mGradientTex;
	gl::GlslProgRef mShader;
	gl::FboRef mRenderTarget;
};