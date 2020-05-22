/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxPd.h"
#include "opensl_stream/opensl_stream.h"

// a namespace for the Pd types
using namespace pd;

class testApp : public ofxAndroidApp, public PdReceiver {

	public:

		void setup();
		void update();
		void draw();
		void exit();
		void reloadTextures();
		void unloadTextures();
		
		void keyPressed  (int key);
		virtual void touchDown(int x, int y, int id);
		virtual void touchMoved(int x, int y, int id);
		virtual void touchUp(int x, int y, int id);
		virtual void touchDoubleTap(int x, int y, int id) {};
		virtual void touchCancelled(int x, int y, int id) {};
		virtual bool backPressed(){
			ofLogNotice("testApp", "back pressed");
			exit();
			ofExit();
			return true;
		}

		void windowResized(int w, int h);
		
		void audioReceived(float * input, int bufferSize, int nChannels);
		void audioRequested(float * output, int bufferSize, int nChannels);
		
		// pd message receiver callbacks
		void print(const std::string& message);
		
		void receiveBang(const std::string& dest);
		void receiveFloat(const std::string& dest, float value);
		void receiveSymbol(const std::string& dest, const std::string& symbol);
		void receiveList(const std::string& dest, const List& list);
		void receiveMessage(const std::string& dest, const std::string& msg, const List& list);

		OPENSL_STREAM *os;
		ofxPd puda;
		static void opensl_process(void *app, int sample_rate, int buffer_frames,
			int input_channels, const short *input_buffer,
			int output_channels, short *output_buffer);

};
