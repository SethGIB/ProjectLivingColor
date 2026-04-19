/*LivingColorFXSender.h
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
#include "cinder/Serial.h"
#include "CinderOpenCV.h"
#include "librealsense2/rs.hpp"
#include "asio/asio.hpp"
#include "Plasma.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FXLed
{
public:
	FXLed() : mPos(vec2()) {}
	FXLed(vec2 _pos) : mPos(_pos) {}
	FXLed(float _x, float _y, Color _col) : mPos(vec2(_x, _y)), mColor(Color8u(_col)) {}
	FXLed(float _x, float _y, int _rx, int _ry, Color _col) : mPos(vec2(_x, _y)), mColor(Color8u(_col)), mRads(vec2(_rx,_ry)) {}
	vec2 getPos() const { return mPos; }
	Color8u getColor() const { return mColor; }
	void setColor(const Color8u& color) { mColor = Color8u(color); }

	void show() const {
		Colorf fColor = Colorf(mColor);
		Color col = mActive ? fColor : fColor * 0.25;
		gl::color(col);
		//gl::drawSolidEllipse(mPos, mRads.x, mRads.y);
		gl::drawSolidRect(Rectf(mPos - mRads, mPos + mRads));
	}

	void activate(bool isActive) { mActive = isActive; }	
	bool isActive() const { return mActive; }
private:
	vec2 mPos;
	vec2 mRads;
	Color8u mColor;
	bool mActive = false;
};

class FXSender {
public:
	FXSender() : mSocket(mIoContext) {}

	// Replaced 'port' (COM) and 'bps' with 'ip' and network 'port'
	void init(const string& ip, const int& port) {
		try {
			// Resolve the remote destination
			asio::ip::udp::resolver resolver(mIoContext);
			mEndpoint = *resolver.resolve(asio::ip::udp::v4(), ip, std::to_string(port)).begin();

			// Only open if not already open
			if (!mSocket.is_open()) {
				mSocket.open(asio::ip::udp::v4());
			}
			mFxBuffer.resize(mFrameSize);
			mPortIsOpen = true;
		}
		catch (const std::exception& e) {
			ci::app::console() << "Network Init Error: " << e.what() << std::endl;
			mPortIsOpen = false;
		}
	}

	bool autoInit(const int& targetPort, const int& discoveryPort = 50050) {
		try {
			asio::ip::udp::socket sock(mIoContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
			sock.set_option(asio::socket_base::broadcast(true));

			asio::ip::udp::endpoint broadcastEndpoint(asio::ip::address_v4::broadcast(), discoveryPort);
			std::string msg = "DISCOVER";
			sock.send_to(asio::buffer(msg), broadcastEndpoint);

			char reply[128];
			asio::ip::udp::endpoint senderEndpoint;
			
			bool received = false;
			sock.async_receive_from(asio::buffer(reply), senderEndpoint,
				[&received](const asio::error_code& error, std::size_t bytes_recvd) {
					if (!error && bytes_recvd > 0) {
						received = true;
					}
				});

			mIoContext.restart();
			mIoContext.run_for(std::chrono::milliseconds(2000));

			if (received) {
				std::string replyStr(reply);
				if (replyStr.find("PLASMA_SERVER") != std::string::npos) {
					std::string ip = senderEndpoint.address().to_string();
					ci::app::console() << "Discovered server at " << ip << std::endl;
					init(ip, targetPort);
					return true;
				}
			}
			ci::app::console() << "Discovery timed out or failed." << std::endl;
			return false;
		}
		catch (const std::exception& e) {
			ci::app::console() << "Discovery error: " << e.what() << std::endl;
			return false;
		}
	}

	void sendFrame(const vector<FXLed>& src) {
		if (!mPortIsOpen) return;

		uint8_t counter = 0;
		for (const FXLed& l : src)
		{
			if (counter >= src.size() || (counter * 3 + 2) >= mFxBuffer.size())
				break;

			Color8u col = l.isActive() ? Color8u(l.getColor()) : (Color8u(l.getColor() / 8));
			mFxBuffer[counter * 3 + 0] = col.r;
			mFxBuffer[counter * 3 + 1] = col.g;
			mFxBuffer[counter * 3 + 2] = col.b;
			counter++;
		}

		// Send the payload over UDP to the receiver
		asio::error_code err;
		mSocket.send_to(asio::buffer(mFxBuffer.data(), mFrameSize), mEndpoint, 0, err);

		if (err) {
			ci::app::console() << "Send Error: " << err.message() << std::endl;
			close();
		}
	}

	void close() {
		if (mPortIsOpen)
		{
			asio::error_code err;
			mSocket.close(err);
			mPortIsOpen = false;
		}
	}

	bool isPortOpen() const { return mPortIsOpen; }
	int mNumLeds = 216;
	int mFrameSize = mNumLeds * 3;

	void checkConnection() {
		// Use a separate io_context so we don't race with the main thread's mIoContext
		std::thread([this]() {
			try {
				asio::io_context discoverContext;
				asio::ip::udp::socket sock(discoverContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
				sock.set_option(asio::socket_base::broadcast(true));

				asio::ip::udp::endpoint broadcastEndpoint(asio::ip::address_v4::broadcast(), 50050);
				std::string msg = "DISCOVER";
				sock.send_to(asio::buffer(msg), broadcastEndpoint);

				char reply[128];
				asio::ip::udp::endpoint senderEndpoint;

				bool received = false;
				sock.async_receive_from(asio::buffer(reply), senderEndpoint,
					[&received](const asio::error_code& error, std::size_t bytes_recvd) {
						if (!error && bytes_recvd > 0) {
							received = true;
						}
					});

				discoverContext.run_for(std::chrono::milliseconds(2000));

				if (received) {
					std::string replyStr(reply);
					if (replyStr.find("PLASMA_SERVER") != std::string::npos) {
						std::string ip = senderEndpoint.address().to_string();
						// Only update the endpoint, don't re-open the socket
						asio::ip::udp::resolver resolver(mIoContext);
						mEndpoint = *resolver.resolve(asio::ip::udp::v4(), ip, std::to_string(50051)).begin();
						ci::app::console() << "Verified server at " << ip << std::endl;
					}
				}
			}
			catch (const std::exception& e) {
				ci::app::console() << "Check connection error: " << e.what() << std::endl;
			}
		}).detach();
	}

private:
	// ASIO Networking components
	asio::io_context mIoContext;
	asio::ip::udp::socket mSocket;
	asio::ip::udp::endpoint mEndpoint;

	vector<uint8_t> mFxBuffer;
	bool mPortIsOpen = false;
};

enum class DrawMode
{
	MAIN,
	DEBUG_COLOR,
	DEBUG_GRAY,
	DEBUG_BINARY,
	DEBUG_CONTOURS,
	DEBUG_PLASMA,
	DEBUG_LEDSONLY,
	COUNT
};

class LivingColorFXSenderApp : public App {
public:
	void setup() override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
	void cleanup() override;

	// I know this is silly but it makes it easy for settings
	static int sWindowWidth;
	static int sWindowHeight;
	static float sFPS;

private:
	void setupCamera();
	void setupLeds();
	void setupImages();
	void setupPlasma();

	void updateFrames(); //get depth data, post process (rot, thresh), update cv::Mat, scale, find contours
	void updateLeds(); //check if each led is inside any contour, set color accordingly, send to fx
	void swapPlasma(); //switch to next plasma in list every kPlasmaSwitchInterval seconds

	void drawLeds();
	void drawMain();	//default drawing mode

	void debugDrawColor();
	void debugDrawCv(int mode); //draw gray or binary
	void debugDrawContours();

	void demoMode(const bool& enable);

	bool mUseNetwork;
	bool mUseCamera;

	int mImgWidth;
	int mImgHeight;

	string mIpAddr;
	int mPortNum;

	double mPlasmaSwitchInterval;
	double mMinContourArea;
	double mDemoSwitchInterval;

	int mNumLedsX;
	int mNumLedsY;
	int mLedWidth;
	int mLedHeight;

	Timer mDemoTimer;
	bool mDemoMode;
	
	Timer mReconnectTimer;

	vector<FXLed> mLeds;
	
	PlasmaRef mPlasmaClassic;
	PlasmaRef mPlasmaTech;
	PlasmaRef mPlasmaPsychedelic;
	PlasmaRef mPlasmaNeo;
	PlasmaRef mCurrentPlasma;
	
	vector<PlasmaRef> mPlasmas;
	Timer mPlasmaTime;

	rs2::pipeline mRs;
	rs2::config mRsConfig;
	rs2::rotation_filter mRsRotFilter;
	rs2::threshold_filter mRsThreshFilter;
	rs2::colorizer mRsColorizer;

	vector<vector<cv::Point>> mContours;
	cv::Mat mDepthMat;
	cv::Mat mGrayMat;
	cv::Mat mBinaryMat;
	cv::Mat mContourMat;

	gl::Texture2dRef mColorTex;
	gl::Texture2dRef mDepthTex;
	gl::Texture2dRef mGrayTex;
	gl::Texture2dRef mBinaryTex;
	gl::Texture2dRef mContoursTex;

	DrawMode mDrawMode;

	FXSender mFxSender;
};