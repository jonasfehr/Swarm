#pragma once

#include "ofMain.h"
#include "GpuParticles.h"
#include "ofxGui.h"
#include "ofxSyphonUtils.h"
#include "ofxOscParameterSync.h"

#define _PROGRAMMABLE_RENDERER

#define HOST "localhost"
#define PORT_SEND 9001
#define PORT_RECEIVE 9000

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
private:

    // set any update uniforms in this function
    void onParticlesUpdate(ofShader& shader);
    
    GpuParticles particles;
    ofEasyCam cam;
//    ofCamera cam;

    ofxPanel gui;
    
    ofxSyphonClientDir syphonInNoise;
    ofxSyphonClientDir syphonInBorder;
    ofxSyphonFbo syphonOut;

    ofParameter<float> trace{"trace", 1.0f, 0.f, 1.0f};
    
    ofxOscParameterSync oscParamSync;
    
    
    
};
