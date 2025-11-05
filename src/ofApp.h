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



		ofBoxPrimitive box;
		ofLight pointLight;
		ofMaterial boxMaterial;



		//color gradient
		int paletteSize;
		vector <ofColor> palette;
		float colorScale, colorSpeed, elapsedTime;

		//automatic color change
		float colorChangeInterval, lastColorChangeTime;
		ofVec2f colorNoiseOffset;  //fixed offset so gradient doesn't move

		//smooth transitions for perlin parameters
		float perlinRangeTarget, perlinHeightTarget, perlinLerpSpeed;
		//animated mesh noise oveer time
		float meshNoiseTime, meshNoiseSpeed;

		// CUBE rotation control - valors entre 0-1 que es mapegen a 360º
		float boxRotNormX = 0.0f;
		float boxRotNormY = 0.0f;
		float boxRotNormZ = 0.0f;
		float boxMaxAngle = 360.0f; // grados: 0..1 -> 0..boxMaxAngle

		// helper para fijar valores (0..1)
		void setBoxRotationNormalized(float x, float y, float z);

		ofColor samplePalette(float t);
		float fbm(float x, float y, int octaves = 4, float persistence = 0.5f);

		//helpers
		void generateRandomPalette();
		float averageHueOfPalette();
};
