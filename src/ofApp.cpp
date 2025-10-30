#include "ofApp.h"

vector<ofColor> palette;

//--------------------------------------------------------------
void ofApp::setup(){

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
	colorSpeed = 0.5f; //vel of gradient's animaiton
	elapsedTime = 0.0f;

	ofBackground(255); //now white change it later if the colours didn't appreciate corerctly
	mainCam.setPosition(0, 0, 80);//initial position for the 3D viewer

	//make the points inside the mesh
	//use a 3d vector to omplete the 3d location os each vertex

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			mainMesh.addVertex(ofPoint(x - width / 2, y - height / 2, 0));
			mainMesh.addColor(ofFloatColor(0, 0, 0)); //placeholder for colour data, we'll get this for the camera
		}
	}

	//join the vertices towether to ake rows of triangles to make the wireframe grid
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
	palette.clear();
	for (int i = 0; i < paletteSize; i++) {
		ofColor c = ofColor::fromHsb(ofRandom(0, 255), (int)ofRandom(140, 255), (int)ofRandom(160, 255));
		palette.push_back(c);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	elapsedTime = ofGetElapsedTimef();

	//Change the z value for each vertex in the mesh
	if (b_messyMesh) {
		for (int i = 0; i < mainMesh.getNumVertices(); i++) {
			ofVec3f newPosition = mainMesh.getVertex(i); //get the current x, y, z position for these vertex
			newPosition.z = ofRandom(-1.0, 1.0); //set a random z position
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
				newPosition.z = ofNoise(ofMap(x, 0, width, 0, perlinRange),
									ofMap(y, 0, height, 0, perlinRange))
					* perlinHeight;
				mainMesh.setVertex(i, newPosition);
				//update the position of the vertex with the new
				i++;
			}
		}
	}

	//update vertex colors using PerlinNoise and animated with time
	//treat each vertex x, y mapped to 0...1 as the smpling doamin
	int nVerts = mainMesh.getNumVertices();
	for (int i = 0; i < nVerts; i++) {
		ofVec3f v = mainMesh.getVertex(i);

		//map vertex x, y to 0...1 based on mesh grid
		float sx = (v.x + (width / 2.0f)) / (float)width;
		float sy = (v.y + (height / 2.0f)) / (float)height;

		//sample coordinates with scale and animated shift
		float sampleX = sx * colorScale + elapsedTime * colorSpeed;
		float sampleY = sy * colorScale;

		//use fbm for richer noise
		float n = fbm(sampleX, sampleY, 5, 0.5f);

		//convert noise to color via palette sampling
		ofColor c = samplePalette(n);
		//Assign color to mesh vertex
		mainMesh.setColor(i, c);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	mainCam.begin();

	if (b_drawWireFrame) {
		//draw wireframe i black while also drawing the filled mesh
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
		case OF_KEY_UP:
			perlinRange += 1.0; 
			break;

		case OF_KEY_DOWN:
			if (perlinRange > 1.0) {
				perlinRange -= 1.0;
			}
			break;

		//increase or decrease the height of the perlin noise waves
		case OF_KEY_RIGHT:
			perlinHeight += 1.0;
			break;

		case OF_KEY_LEFT:
			if (perlinHeight > 1.0) {
				perlinHeight -= 1.0;
			}
			break;

		case 'r':
			//randomize palete
			palette.clear();
			for (int i = 0; i < paletteSize; i++) {
				ofColor c = ofColor::fromHsb(ofRandom(0, 255), (int)ofRandom(140, 255), (int)ofRandom(160, 255));
				palette.push_back(c);
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
