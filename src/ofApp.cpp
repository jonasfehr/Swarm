#include "ofApp.h"

//--------------------------------------------------------------
//glm::vec2 inputSizeX = glm::vec2(5,10);
//
//glm::vec2 indexToTexCoord(int index){
//    glm::vec2 texCoord;
//    texCoord.x = glm::mod(index, inputSizeX.x);
//    texCoord.y = (index-texCoord.x) / inputSizeX.x;
//    return texCoord;
//}
//
//int texCoordToIndex(glm::vec2 texCoord){
//    int index;
//    index = int(texCoord.y*inputSizeX.x+texCoord.x);
//    return index;
//}

void ofApp::setup()
{
//        for(int y = 0; y < 10; y++){
//            for(int x = 0; x < 5; x++){
//
//        cout << x << " " << y<< " | ";
//            int i = texCoordToIndex(glm::vec2(x,y));
//            cout << i  << " | ";
//
//        glm::vec2 coord = indexToTexCoord(i);
//        cout << coord.x << " " << coord.y << endl;
//        cout << endl;
//    }
//    }

    syphonOut.setup("render", 1024, 1024);

    ofBackground(0);
    ofSetFrameRate(60);
    
    // 1,000,000 particles
    unsigned w = 600;
    unsigned h = 2;
    
    
    particles.init(w, h);
    
    particles.loadShaders("shaders/update", "shaders/render");
    
    // initial positions
    // use new to allocate 4,000,000 floats on the heap rather than
    // the stack
    float* particlePosns = new float[w * h * 4];
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            unsigned idx = y * w + x;
            particlePosns[idx * 4] = 400.f * x / (float)w - 200.f; // particle x
            particlePosns[idx * 4 + 1] = 400.f * y / (float)h - 200.f; // particle y
            particlePosns[idx * 4 + 2] = 0.f; // particle z
            particlePosns[idx * 4 + 3] = 0.f; // dummy
        }
    }
    particles.loadDataTexture(GpuParticles::POSITION, particlePosns);
    delete[] particlePosns;
    
    // initial velocities
    particles.zeroDataTexture(GpuParticles::VELOCITY);
    
    // listen for update event to set additonal update uniforms
    ofAddListener(particles.updateEvent, this, &ofApp::onParticlesUpdate);
    
    gui.setup();
    gui.setName("Swarm");
    particles.parameters.getGroup("Visual").add(trace);
    gui.add(particles.parameters);
    
//    cam.lookAt(glm::vec3(0,0,0));
//    cam.setPosition(glm::vec3(0,0,0));
    cam.enableOrtho();
    cam.setNearClip(0.);
//        cam.disableMouseInput();

    
    syphonInBorder.setup("Border", "MadMapper");
    particles.setBorderTexture(syphonInBorder.getTexture());
    
    syphonInNoise.setup("NoiseInput", "MadMapper");
    particles.setNoiseTexture(syphonInNoise.getTexture());


    
    // OSC
    oscParamSync.setup(particles.parameters, PORT_RECEIVE, HOST, PORT_SEND);
}

//--------------------------------------------------------------
void ofApp::update()
{
    particles.update();
    
    oscParamSync.update();
}

// set any update uniforms in this function
void ofApp::onParticlesUpdate(ofShader& shader)
{
    ofVec3f mouse(ofGetMouseX() - .5f * ofGetWidth(), .5f * ofGetHeight() - ofGetMouseY() , 0.f);
    shader.setUniform3fv("mouse", mouse.getPtr());
    shader.setUniform1f("elapsed", ofGetLastFrameTime());
    shader.setUniform1f("radiusSquared", 200.f * 200.f);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    syphonOut.begin();
    {
        ofSetColor(ofColor::black, 255*pow(trace,2));
        ofDrawRectangle(0,0, syphonOut.getWidth(), syphonOut.getHeight());
        cam.begin();
        {
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            particles.draw();
            ofDisableBlendMode();
        }
        cam.end();
    }
    syphonOut.end();

    syphonOut.draw();
    int size = 100;
    syphonInBorder.draw(ofGetWidth()-size*2., ofGetHeight()-size, size,size);
    syphonInNoise.draw(ofGetWidth()-size, ofGetHeight()-size, size,size);

    gui.draw();
    
    syphonOut.publish();
    
    stringstream windowInfo;
    windowInfo << " | Syphon (b): " << syphonInBorder.getName();
    windowInfo << " | Syphon (n): " << syphonInNoise.getName();
    windowInfo << " | FPS: " << fixed << setprecision(1) << ofGetFrameRate();
    
    ofSetWindowTitle(windowInfo.str());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'b') syphonInBorder.next();
    if(key == 'n') syphonInNoise.next();

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
