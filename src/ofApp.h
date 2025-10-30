#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//New mesh object
		ofMesh mainMesh;
		ofEasyCam mainCam;
		int width, height;
		bool b_messyMesh, b_perlinMesh, b_drawWireFrame;
		float perlinRange, perlinHeight;

		//color gradient
		int paletteSize;
		vector <ofColor> palette;
		float colorScale, colorSpeed, elapsedTime;

		ofColor samplePalette(float t);
		float fbm(float x, float y, int octaves = 4, float persitence = 0.5f);
};


// Linearly interpolates between colors in the palette based on t in [0,1]
ofColor ofApp::samplePalette(float t) {
	if (palette.empty()) return ofColor::black;
	if (palette.size() == 1) return palette[0];

	// Clamp t to [0,1]
	t = ofClamp(t, 0.0f, 1.0f);

	float scaled = t * (palette.size() - 1);
	int idx0 = static_cast<int>(floor(scaled));
	int idx1 = idx0 + 1;
	float frac = scaled - idx0;

	if (idx1 >= palette.size()) {
		idx1 = palette.size() - 1;
		idx0 = idx1;
		frac = 0.0f;
	}

	return palette[idx0].getLerped(palette[idx1], frac);
}
