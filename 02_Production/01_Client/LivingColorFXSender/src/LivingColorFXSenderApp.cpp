/*LivingColorFXSenderApp.cpp
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

#include "LivingColorFXSender.h"

int LivingColorFXSenderApp::sWindowWidth = 360;
int LivingColorFXSenderApp::sWindowHeight = 640;
float LivingColorFXSenderApp::sFPS = 30.0f;

void LivingColorFXSenderApp::setup()
{
	mUseNetwork = true;
	mUseCamera = false;

	mImgHeight = 640;
	mImgWidth = 360;

	mNumLedsX = 12;
	mNumLedsY = 18;
	mLedWidth = ceil(LivingColorFXSenderApp::sWindowWidth / (float)(mNumLedsX * 2));
	mLedHeight = ceil(LivingColorFXSenderApp::sWindowHeight / (float)(mNumLedsY * 2));


	mIpAddr = "10.0.2.232"; //"192.168.0.8"; 
	mPortNum = 50051;

	mPlasmaSwitchInterval = 6.0;
	mMinContourArea = 40000.0;
	mDemoSwitchInterval = 3.26577;

	mDrawMode = DrawMode::MAIN;
	mDemoMode = true;

	if (mUseCamera)
	{
		setupCamera();
		setupImages();
	}

	if (mUseNetwork) {
		// Attempt discovery, and if it fails or isn't found within 2 seconds, print an error
		if (!mFxSender.autoInit(mPortNum)) {
			// Optional: Fallback to static IP or retry. We will just report error here.
			ci::app::console() << "Could not auto-discover plasma server on the network." << std::endl;
		}
	}

	setupLeds();
	setupPlasma();

	mCurrentPlasma = mPlasmaClassic;
	mCurrentPlasma->setActive(true);

	mPlasmaTime = Timer(true);
	mDemoTimer = Timer(true);
	mReconnectTimer = Timer(true);
}

void LivingColorFXSenderApp::keyDown(KeyEvent event)
{
	if (event.getChar() == '0')
	{
		mDrawMode = DrawMode::MAIN;
	}
	else if (event.getChar() == '1')
	{
		mDrawMode = DrawMode::DEBUG_COLOR;
	}
	else if (event.getChar() == '2')
	{
		mDrawMode = DrawMode::DEBUG_GRAY;
	}
	else if(event.getChar() == '3')
	{
		mDrawMode = DrawMode::DEBUG_BINARY;
	}
	else if (event.getChar() == '4')
	{
		mDrawMode = DrawMode::DEBUG_CONTOURS;
	}
	else if (event.getChar() == '5')
	{
		mDrawMode = DrawMode::DEBUG_PLASMA;
	}
	else if (event.getChar() == '6')
	{
		mDrawMode = DrawMode::DEBUG_LEDSONLY;
	}

	else if (event.getChar() == 'd' || event.getChar() == 'D')
	{
		mDemoMode = !mDemoMode;
		demoMode(mDemoMode);
	}
}

void LivingColorFXSenderApp::update()
{
	if (mUseNetwork) {
		if (!mFxSender.isPortOpen()) {
			if (mReconnectTimer.getSeconds() > 5.0) {
				ci::app::console() << "Attempting to reconnect..." << std::endl;
				mFxSender.autoInit(mPortNum);
				mReconnectTimer.start(); // restart the timer
			}
		} else {
			// If we think we are connected, verify periodically (e.g. every 10 seconds)
			// This catches scenarios where the server rebooted under a new IP address
			if (mReconnectTimer.getSeconds() > 10.0) {
				mFxSender.checkConnection();
				mReconnectTimer.start();
			}
		}
	}

	if (mUseCamera)
	{
		updateFrames();
	}
	updateLeds();
	swapPlasma();

	if (mDemoMode)
	{
		if (mDemoTimer.getSeconds() >= mDemoSwitchInterval)
		{
			int nextMode = (static_cast<int>(mDrawMode) + 1) % static_cast<int>(DrawMode::COUNT);
			mDrawMode = static_cast<DrawMode>(nextMode);

			mDemoTimer.start();
		}
	}
}

void LivingColorFXSenderApp::draw()
{
	gl::clear(Color::black());
	gl::setMatricesWindow(getWindowSize());
	mCurrentPlasma->render(ivec2(mImgWidth, mImgHeight), Area(0, 0, LivingColorFXSenderApp::sWindowWidth, LivingColorFXSenderApp::sWindowHeight));
	switch (mDrawMode)
	{
		case DrawMode::MAIN:
			drawMain();
			break;
		case DrawMode::DEBUG_COLOR:
			debugDrawColor();
			break;
		case DrawMode::DEBUG_GRAY:
			debugDrawCv(0);
			break;
		case DrawMode::DEBUG_BINARY:
			debugDrawCv(1);
			break;
		case DrawMode::DEBUG_CONTOURS:
			debugDrawContours();
			break;
		case DrawMode::DEBUG_PLASMA:
			gl::draw(mCurrentPlasma->getTexture());
			break;
		case DrawMode::DEBUG_LEDSONLY:
			drawLeds();
			break;
	}	
}

void LivingColorFXSenderApp::cleanup()
{
	if (mUseCamera)
	{
		mRs.stop();
	}
	if (mUseNetwork)
	{
		mFxSender.close();
	}
}

void LivingColorFXSenderApp::setupCamera()
{
	mRsConfig.enable_stream(RS2_STREAM_COLOR, mImgHeight, mImgWidth, RS2_FORMAT_RGB8, 30);
	mRsConfig.enable_stream(RS2_STREAM_DEPTH, mImgHeight, mImgWidth, RS2_FORMAT_Z16, 30);
	vector<rs2_stream> streams = { RS2_STREAM_DEPTH, RS2_STREAM_COLOR };
	mRsRotFilter = rs2::rotation_filter(streams);
	mRsRotFilter.set_option(RS2_OPTION_ROTATION, -90.0f);

	mRsThreshFilter = rs2::threshold_filter(0.15f, 2.0f);

	mRsColorizer = rs2::colorizer(4);
	mRs.start(mRsConfig);
}

void LivingColorFXSenderApp::setupLeds()
{
	int count = 0;
	for (int y = 0; y < mNumLedsY; y++)
	{
		float y0 = lmap<float>(y + 0.5f, 0, mNumLedsY, 0, LivingColorFXSenderApp::sWindowHeight);
		for (int x = 0; x < mNumLedsX; x++)
		{
			float x0 = -1;
			if (y % 2 == 0)
			{
				x0 = lmap<float>(x + 0.5f, 0, mNumLedsX, 0, LivingColorFXSenderApp::sWindowWidth);
			}
			else
			{
				x0 = lmap<float>(mNumLedsX - (x + 0.5f), 0, mNumLedsX, 0, LivingColorFXSenderApp::sWindowWidth);
			}

			auto ledColor = Color8u(64,64,64);
			mLeds.push_back(FXLed(x0, y0, mLedWidth, mLedHeight, ledColor));
			count += 1;
		}
	}
}

void LivingColorFXSenderApp::setupImages()
{
	mColorTex = gl::Texture2d::create(mImgWidth, mImgHeight, gl::Texture2d::Format().internalFormat(GL_RGB).dataType(GL_UNSIGNED_BYTE));
	mDepthTex = gl::Texture2d::create(mImgWidth, mImgHeight, gl::Texture2d::Format().internalFormat(GL_RGB).dataType(GL_UNSIGNED_BYTE));
	mGrayTex = gl::Texture2d::create(mImgWidth, mImgHeight, gl::Texture2d::Format().internalFormat(GL_RED).dataType(GL_UNSIGNED_BYTE));
	mBinaryTex = gl::Texture2d::create(mImgWidth, mImgHeight, gl::Texture2d::Format().internalFormat(GL_RED).dataType(GL_UNSIGNED_BYTE));
	mContoursTex = gl::Texture2d::create(mImgWidth, mImgHeight);

	mDepthMat = cv::Mat::zeros(mImgHeight, mImgWidth, CV_8UC3);
	mGrayMat = cv::Mat::zeros(mImgHeight, mImgWidth, CV_8UC1);
	mBinaryMat = cv::Mat::zeros(mImgHeight, mImgWidth, CV_8UC1);
	mContourMat = cv::Mat::zeros(mImgHeight, mImgWidth, CV_8UC4);
}

void LivingColorFXSenderApp::setupPlasma()
{
	mPlasmaClassic = Plasma::create();
	mPlasmaClassic->init("shaders/passthru.vert", "shaders/plasma_classic.frag", "textures/classic/TX_Noise_Classic.png", "textures/classic/TX_Grad_Classic.png", ivec2(mImgWidth, mImgHeight));

	mPlasmaNeo = Plasma::create();
	mPlasmaNeo->init("shaders/passthru.vert", "shaders/plasma_neo.frag", "textures/neo/TX_Noise_Neo.png", "textures/neo/TX_Grad_Neo.png", ivec2(mImgWidth, mImgHeight));

	mPlasmaTech = Plasma::create();
	mPlasmaTech->init("shaders/passthru.vert", "shaders/plasma_tech.frag", "textures/tech/TX_Noise_Tech.png", "textures/tech/TX_Grad_Tech.png", ivec2(mImgWidth, mImgHeight));
	
	mPlasmaPsychedelic = Plasma::create();
	mPlasmaPsychedelic->init("shaders/passthru.vert", "shaders/plasma_psyche.frag", "textures/psyche/TX_Noise_Psyche.png", "textures/psyche/TX_Grad_Psyche.png", ivec2(mImgWidth, mImgHeight));
}

void LivingColorFXSenderApp::updateFrames()
{
	auto frames = mRs.wait_for_frames();
	if (mDrawMode == DrawMode::DEBUG_COLOR)
	{
		auto color_frame = frames.get_color_frame().apply_filter(mRsRotFilter);
		mColorTex->update(color_frame.get_data(), GL_RGB, GL_UNSIGNED_BYTE, 0, mImgWidth, mImgHeight);
	}

	auto depth_frame = frames.get_depth_frame().apply_filter(mRsRotFilter).apply_filter(mRsThreshFilter).apply_filter(mRsColorizer);
	mDepthMat.data = (uchar*)depth_frame.get_data();
	mDepthTex->update(mDepthMat.data, GL_RGB, GL_UNSIGNED_BYTE, 0, mImgWidth, mImgHeight);

	mContours.clear();
	mContourMat.setTo(cv::Scalar(0, 0, 0, 0));

	cv::cvtColor(mDepthMat, mGrayMat, cv::COLOR_RGB2GRAY);
	cv::threshold(mGrayMat, mBinaryMat, 4, 255, cv::THRESH_BINARY);

	//rotate the binary mat
	cv::Mat rotatedBinaryMat;
	cv::flip(mBinaryMat, rotatedBinaryMat, 0); // flip around x-axis (vertical flip)
	cv::findContours(rotatedBinaryMat, mContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	vector<vector<cv::Point>> filteredContours;
	const double minContourArea = mMinContourArea;

	for (const auto& contour : mContours) {
		if (cv::contourArea(contour) >= minContourArea) {
			filteredContours.push_back(contour);
		}
	}
	mContours = filteredContours;
	// --------------------------

	cv::drawContours(mContourMat, mContours, -1, cv::Scalar(255, 64, 0, 255), 4);

	mContoursTex->update(mContourMat.data, GL_RGBA, GL_UNSIGNED_BYTE, 0, mImgWidth, mImgHeight);
	if(mDrawMode == DrawMode::DEBUG_GRAY)
	{
		mGrayTex->update(mGrayMat.data, GL_RED, GL_UNSIGNED_BYTE, 0, mImgWidth, mImgHeight);
	}
	if(mDrawMode == DrawMode::DEBUG_BINARY)
	{
		mBinaryTex->update(mBinaryMat.data, GL_RED, GL_UNSIGNED_BYTE, 0, mImgWidth, mImgHeight);
	}
}

void LivingColorFXSenderApp::updateLeds()
{
	auto surf = mCurrentPlasma->getPixels();
	for (FXLed& led : mLeds)
	{
		//auto lPos = led.getPos();
		//ivec2 scaledPos = ivec2(lmap<float>(lPos.x, 0, kWindowWidth, 0, kWidth), lmap<float>(lPos.y, 0, kWindowHeight, 0, kHeight));
		//ivec2 scaledPos ()
		auto pixelColor = surf.getPixel(ivec2(led.getPos()));
		led.setColor(Color8u(pixelColor.r, pixelColor.g, pixelColor.b));
		if (mUseCamera && mContours.size() > 0)
		{
			led.activate(false);
			bool isInsideAnyContour = false;
			for (auto contour : mContours)
			{
				if (cv::pointPolygonTest(contour, cv::Point2f(led.getPos().x, led.getPos().y), false) >= 0)
				{
					led.activate(true);
					break;
				}
			}
		}
		else
		{
			led.activate(true);
		}
	}
	if (mUseNetwork && mFxSender.isPortOpen())
	{
		mFxSender.sendFrame(mLeds);
	}
}

void LivingColorFXSenderApp::swapPlasma()
{
	if (mPlasmaTime.getSeconds() >= mPlasmaSwitchInterval)
	{
		mCurrentPlasma->setActive(false);
		mPlasmaTime.stop();
		if (mCurrentPlasma == mPlasmaClassic)
		{
			mCurrentPlasma = mPlasmaTech;
		}
		else if (mCurrentPlasma == mPlasmaTech)
		{
			mCurrentPlasma = mPlasmaPsychedelic;
		}
		else if (mCurrentPlasma == mPlasmaPsychedelic)
		{
			mCurrentPlasma = mPlasmaNeo;
		}
		else
		{
			mCurrentPlasma = mPlasmaClassic;
		}
		mCurrentPlasma->setActive(true);
		mPlasmaTime.start();
	}
}

void LivingColorFXSenderApp::drawLeds()
{
	for (const FXLed& led : mLeds)
	{
		led.show();
	}
}

void LivingColorFXSenderApp::drawMain()
{
	drawLeds();

	gl::color(Color::white());
	gl::enableAlphaBlending(true);
	gl::pushMatrices();
	gl::scale(vec2(1, -1));
	gl::translate(vec2(0, -mImgHeight));
	gl::draw(mContoursTex);
	gl::popMatrices();
	gl::disableAlphaBlending();

}

void LivingColorFXSenderApp::debugDrawColor()
{
	gl::color(Color::white());
	gl::draw(mColorTex);
}

void LivingColorFXSenderApp::debugDrawCv(int mode)
{
	gl::color(Color::white());
	switch (mode)
	{
		case 0:
			gl::draw(mGrayTex);
			break;
		case 1:
			gl::draw(mBinaryTex);
			break;
	}
}

void LivingColorFXSenderApp::debugDrawContours()
{
	gl::color(Color::white());
	gl::draw(mDepthTex);
	gl::enableAlphaBlending(true);
	gl::pushMatrices();
	gl::scale(vec2(1, -1));
	gl::translate(vec2(0, -mImgHeight));
	gl::draw(mContoursTex);
	gl::popMatrices();
	gl::enableAlphaBlending(false);

}

void LivingColorFXSenderApp::demoMode(const bool& enable)
{
	if (enable)
	{
		mDemoTimer.start();
	}
	else
	{
		mDemoTimer.stop();
		mDrawMode = DrawMode::MAIN;
	}
}

void prepareSettings(App::Settings* settings)
{
	settings->setWindowSize(LivingColorFXSenderApp::sWindowWidth, LivingColorFXSenderApp::sWindowHeight);
	settings->setFrameRate(LivingColorFXSenderApp::sFPS);
	//settings->setConsoleWindowEnabled( true );

}

CINDER_APP( LivingColorFXSenderApp, RendererGl, *prepareSettings )