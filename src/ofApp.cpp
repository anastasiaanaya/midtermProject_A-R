#include "ofApp.h"

vector<ofColor> palette;

//--------------------------------------------------------------
void ofApp::setup(){

	pointLight.setup();
	pointLight.setPosition(100, 100, 100);
	pointLight.enable();
	ofEnableLighting();
	ofEnableDepthTest();




	//width and height for our mesh and initial rendering values
	width = 50;
	height = 50;

	//rendering styles --> false
	b_messyMesh = false;
	b_perlinMesh = false;
	b_drawWireFrame = true;

	//Set the initial values to use for the perlin noise
	perlinRange = 1.0;
	perlinHeight = 5.0;

	//Color gradienft parameters
	paletteSize = 6; //how many colors has the gradient
	colorScale = 3.0f; //to control the noise that controls the color
	//colorSpeed = 0.5f; //level of gradient's animaiton
	elapsedTime = 0.0f;

	//automatic color switching
	colorChangeInterval = 5.0f; //every 5 seconds (adjust as needed)
	lastColorChangeTime = -9999.0f;
	//fixed random offset --> the gradient doesn't move
	colorNoiseOffset.set(ofRandom(0, 1000.0f), ofRandom(0, 1000.0f));

	//smooth transitions for perlin parameters
	perlinRangeTarget = perlinRange;
	perlinHeightTarget = perlinHeight;
	perlinLerpSpeed = 0.5f;		//how fast the perlin params interpolate to targets
	meshNoiseTime = ofRandom(0.0f, 1000.0f); //radnom for each time is different
	meshNoiseSpeed = 0.3f; //mdorerate speed


	ofBackground(10); //now black change it later if the colours didn't appreciate corerctly
	mainCam.setPosition(0, 0, 80);//initial position for the 3D viewer

	//make the points inside the mesh
	//use a 3d vector to omplete the 3d location os each vertex

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			mainMesh.addVertex(ofPoint(x - width / 2, y - height / 2, 0));
			mainMesh.addColor(ofFloatColor(0, 0, 0)); //placeholder for color data, we'll get this for the camera
		}
	}

	//join the vertices together to ake rows of triangles to make the wireframe grid
	for (int y = 0; y < height - 1; y++) {
		for (int x = 0; x < width - 1; x++) {
			mainMesh.addIndex(x + y * width);				// 0
			mainMesh.addIndex((x + 1) + y * width);			// 1
			mainMesh.addIndex(x + (y + 1) * width);			// 10

			mainMesh.addIndex((x + 1) + y * width);			// 1
			mainMesh.addIndex((x + 1) + (y + 1) * width);	// 11
			mainMesh.addIndex(x + (y + 1) * width);			// 10
		}
	}

	//create random palette
	generateRandomPalette();
	lastColorChangeTime = ofGetElapsedTimef();

	setBoxRotationNormalized(0.25f, 0.5f, 0.0f); // X=90°, Y=180°, Z=0°
}

//--------------------------------------------------------------
void ofApp::update() {


	elapsedTime = ofGetElapsedTimef();
	meshNoiseTime += ofGetLastFrameTime() * meshNoiseSpeed;
	b_perlinMesh = !b_perlinMesh;

	// Automatic palette change based on timer
	if (elapsedTime - lastColorChangeTime >= colorChangeInterval) {
		generateRandomPalette();
		lastColorChangeTime = elapsedTime;

		// when palette changes, set new aesthetic targets for perlin params
		float avgHue = averageHueOfPalette(); // 0..255
		// map hue to ranges that tend to look good:
		// cooler hues -> slightly taller, calmer waves; warmer hues -> more energetic
		float hueNorm = avgHue / 255.0f;
		perlinRangeTarget = ofMap(hueNorm, 0.0f, 1.0f, 1.0f, 6.0f);
		perlinHeightTarget = ofMap(hueNorm, 0.0f, 1.0f, 3.0f, 12.0f);
	}

	// Smoothly interpolate perlin parameters toward the target values
	perlinRange = ofLerp(perlinRange, perlinRangeTarget, ofClamp(perlinLerpSpeed * ofGetLastFrameTime(), 0.01f, 0.5f));
	perlinHeight = ofLerp(perlinHeight, perlinHeightTarget, ofClamp(perlinLerpSpeed * ofGetLastFrameTime(), 0.01f, 0.5f));

	//Change the z value for each vertex in the mesh
	if (b_messyMesh) {
		for (int i = 0; i < mainMesh.getNumVertices(); i++) {
			ofVec3f newPosition = mainMesh.getVertex(i); //get the current x, y, z position for these vertex
			newPosition.z = ofRandom(-1.0f, 1.0f); //set a random z position
			mainMesh.setVertex(i, newPosition); //update the vertex with the new coordinates
		}
	}

	if (b_perlinMesh) {
		//distort the z value of each point in the mesh with perlinNoise
		int i = 0;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				ofVec3f newPosition = mainMesh.getVertex(i);
				//use the ofMap function to map our x,y inputs to a variable output
				// range so we can see different levels of distorsion in the perlinNoise
				// Multiply the z distorsion by perlinHeight to increase the height of the waves
				float nx = ofMap(x, 0, width, 0, perlinRange);
				float ny = ofMap(y, 0, width, 0, perlinRange);
				newPosition.z = ofNoise(nx, ny, meshNoiseTime) * perlinHeight;
				mainMesh.setVertex(i, newPosition);
				//update the position of the vertex with the new
				i++;
			}
		}
	}

	//update vertex colors using PerlinNoise with fixed noise offset
	int nVerts = mainMesh.getNumVertices();
	for (int i = 0; i < nVerts; i++) {
		ofVec3f v = mainMesh.getVertex(i);

		//map vertex x, y to 0...1 based on mesh grid
		float sx = (v.x + (width / 2.0f)) / (float)width;
		float sy = (v.y + (height / 2.0f)) / (float)height;

		//sample coordinates with scale and animated shift
		float sampleX = sx * colorScale + colorNoiseOffset.x;
		float sampleY = sy * colorScale + colorNoiseOffset.y;

		//use fbm for richer noise
		float n = fbm(sampleX, sampleY, 5, 0.5f);
		n = ofClamp(n, 0.0f, 1.0f);

		//convert noise to color via palette sampling
		ofColor c = samplePalette(n);
		//Assign color to mesh vertex
		mainMesh.setColor(i, c);


		//Same color than mesh for the box material
		boxMaterial.setDiffuseColor(c);

	}

	// Animate box rotation based on time
	//float t = ofGetElapsedTimef();
	//elapsedTime
	float nx = (sin(elapsedTime * 0.6f) + 1.0f) * 0.05f; // oscila 0..1
	float ny = (sin(elapsedTime * 0.8f + 1.0f) + 1.0f) * 0.07f;
	float nz = (sin(elapsedTime * 1.1f + 2.0f) + 1.0f) * 0.07f;
	setBoxRotationNormalized(nx, ny, nz);
}

//--------------------------------------------------------------
void ofApp::draw(){

	mainCam.begin();
	ofFill();
	
		
	if (b_drawWireFrame) {
		//draw wireframe black while also drawing the filled mesh
		ofEnableDepthTest();
		//draw filled triangles with vertex colors
		ofSetColor(255); //white to not tint the vertex colors
		mainMesh.draw();

		ofSetColor(0);
		mainMesh.drawWireframe();

		ofDisableDepthTest();
	} else {
		mainMesh.drawVertices();
	}

	boxMaterial.begin();
	ofEnableDepthTest();
	ofPushMatrix();

	// relative poosition
	ofTranslate(0, 0, 4.5);

	// angles calcul (0..1 -> 0..boxMaxAngle)
	float angleX = boxRotNormX * boxMaxAngle;
	float angleY = boxRotNormY * boxMaxAngle;
	float angleZ = boxRotNormZ * boxMaxAngle;

	// box rotation
	ofRotateZDeg(angleZ);
	ofRotateYDeg(angleY);
	ofRotateXDeg(angleX);

	ofScale(0.1, 0.1, 0.1);
	box.draw();

	ofPopMatrix();
	boxMaterial.end();
	mainCam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
		//use a switch statement to see which key is pressed
		case 'f':
			ofToggleFullscreen();
			break;

		case ' ':
			b_messyMesh = !b_messyMesh; //toggle the messy mesh boolean
			break;

		case 'w':
			b_drawWireFrame = !b_drawWireFrame; //toggle the wireframe boolean
			break;

		case 'p':
			b_perlinMesh = !b_perlinMesh; //toggle the perlin noise boolean
			break;

		//increase or decrease the range of the perlin noise value
		//case OF_KEY_UP:
		//	perlinRange += 1.0;
		//	perlinRangeTarget = perlinRange;
		//	break;

		//case OF_KEY_DOWN:
		//	if (perlinRange > 1.0) {
		//		perlinRange -= 1.0;
		//		perlinRangeTarget = perlinRange;
		//	}
		//	break;

		////increase or decrease the height of the perlin noise waves
		//case OF_KEY_RIGHT:
		//	perlinHeight += 1.0;
		//	perlinHeightTarget = perlinHeight;
		//	break;

		//case OF_KEY_LEFT:
		//	if (perlinHeight > 1.0) {
		//		perlinHeight -= 1.0;
		//		perlinHeightTarget = perlinHeight;
		//	}
		//	break;

		case 'r':
			// force new palette immediately (still allowed) and reset timer
			generateRandomPalette();
			lastColorChangeTime = ofGetElapsedTimef();
			// adjust perlin targets with automatic change
			{
				float avgHue = averageHueOfPalette();
				float hueNorm = avgHue / 255.0f;
				perlinRangeTarget = ofMap(hueNorm, 0.0f, 1.0f, 1.0f, 6.0f);
				perlinHeightTarget = ofMap(hueNorm, 0.0f, 1.0f, 3.0f, 12.0f);
			}
			break;

	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

// Fractional Brownian Motion (fbm) implementation using Perlin noise
float ofApp::fbm(float x, float y, int octaves, float persistence) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f; // Used for normalization

    for (int i = 0; i < octaves; i++) {
        total += ofNoise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

// Sample palette (linear interpolation)
ofColor ofApp::samplePalette(float t) {
	if (palette.empty()) return ofColor::black;
	if (palette.size() == 1) return palette[0];

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

// Generate a random palette visually pleasant (saturation/brightness constrained)
void ofApp::generateRandomPalette() {
	palette.clear();
	for (int i = 0; i < paletteSize; i++) {
		ofColor c = ofColor::fromHsb((int)ofRandom(0, 255), (int)ofRandom(140, 255), (int)ofRandom(160, 255));
		palette.push_back(c);
	}

	// When regenerating, optionally change the fixed noise offset slightly to vary the static pattern
	colorNoiseOffset.set(ofRandom(0, 1000.0f), ofRandom(0, 1000.0f));
}

// Compute average hue of the palette (0..255), used to drive perlin parameter targets
float ofApp::averageHueOfPalette() {
	if (palette.empty()) return 0.0f;
	float sumHue = 0.0f;
	for (auto & c : palette) {
		int hue = c.getHue(); // 0..255
		sumHue += hue;
	}
	return sumHue / (float)palette.size();
}

// box rotation
void ofApp::setBoxRotationNormalized(float x, float y, float z) {
	boxRotNormX = ofClamp(x, 0.0f, 1.0f);
	boxRotNormY = ofClamp(y, 0.0f, 1.0f);
	boxRotNormZ = ofClamp(z, 0.0f, 1.0f);
}
