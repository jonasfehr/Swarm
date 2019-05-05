/*
 *  The Code is based on the ofxAddon by NeilMendoza, as it got a lot of modifications to the original,
 *  I choose to include the files in my directory directly. The original can be found here:
 *  https://github.com/neilmendoza/ofxGpuParticles
 *  Thanks a lot ot Neil Mendoza for his fabulous work.
 *
 *  GpuParticles.h
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"

/**
 * For now, only uses RGBA and TEXTURE_RECTANGLE to make usage simpler
 */
class GpuParticles
{
public:
    ofParameter<float> speedBoids{"speedBoids", 0.5f, 0.0f, 1.0f};
    ofParameter<float> numBoids{"numBoids", 1.0f, 0.0f, 1.0f};
    ofParameter<float> cohesion{"cohesion", 0.6f, 0.0f, 1.0f};
    ofParameter<float> separation{"separation", 0.1f, 0.0f, 1.0f};
    ofParameter<float> align{"align", 0.4f, 0.0f, 1.0f};
    ofParameter<float> random{"random", 0.01f, 0.0f, 1.0f};
    ofParameter<float> borderAvoid{"borderAvoid", 0.0f, 0.0f, 1.0f};
    ofParameter<float> noiseSteering{"noiseSteerings", 0.0f, 0.0f, 1.0f};
    ofParameter<float> cohesionDist{"cohesionDist", 0.1f, 0.0f, 1.0f};
    ofParameter<float> separationDist{"separationDist", 0.1f, 0.0f, 1.0f};
    ofParameter<float> alignDist{"alignDist", 0.4f, 0.0f, 1.0f};
    ofParameter<bool> do2D{"do2D", true};
    
    ofParameterGroup parametersFlocking{"Flocking", speedBoids, numBoids, cohesion, separation, align, random, borderAvoid,noiseSteering, cohesionDist, separationDist, alignDist, do2D };
    
    ofParameter<float> pointScale{"pointScale", 0.2f, 0.0001f, 1.0f};
    ofParameter<float> dotSize{"dotSize", 1.0f, 0.f, 1.0f};
    ofParameter<float> feather{"feather", 1.0f, 0.f, 1.0f};
    ofParameter<float> gamma{"gamma", 1.0f, 0.000001f, 2.0f};
    ofParameter<float> transparency{"transparency", 1.0f, 0.f, 1.0f};
    
    ofParameterGroup parametersVisual{"Visual", pointScale, dotSize, feather, gamma,transparency};
    
    ofParameterGroup parameters{"ParticleSystem", parametersFlocking, parametersVisual};
    
    
    static const string UNIFORM_PREFIX;
    static const string UPDATE_SHADER_NAME;
    static const string DRAW_SHADER_NAME;
    static const unsigned FLOATS_PER_TEXEL = 4;
    
    // you don't have to use these but makes
    // code more readable
    enum DataTextureIndex
    {
        POSITION,
        VELOCITY
    };
    
    GpuParticles();
    
    void init(unsigned width, unsigned height,
              ofPrimitiveMode primitive = OF_PRIMITIVE_POINTS, bool loadDefaultShaders = false, unsigned numDataTextures = 2);
    void update();
    void draw();
    
    void loadShaders(const string& updateShaderName, const string& drawShaderName);
    
    void loadDataTexture(unsigned idx, float* data,
                         unsigned x = 0, unsigned y = 0, unsigned width = 0, unsigned height = 0);
    void zeroDataTexture(unsigned idx,
                         unsigned x = 0, unsigned y = 0, unsigned width = 0, unsigned height = 0);
    
    unsigned getWidth() const { return width; }
    unsigned getHeight() const { return height; }
    unsigned getNumFloats() const { return numFloats; }
    
    void setTextureLocation(unsigned textureLocation) { this->textureLocation = textureLocation; }
    
    // listen to these events to set custom uniforms
    ofEvent<ofShader> updateEvent;
    ofEvent<ofShader> drawEvent;
    
    ofVboMesh& getMeshRef() { return mesh; }
    
    // advanced
    ofShader& getUpdateShaderRef() { return updateShader; }
    ofShader& getDrawShaderRef() { return drawShader; }
    
    void save(const string& fileName);
    void load(const string& fileName);
    
    // this will be called for you by ofxGpuParticles::draw()
    // you should only need to call it yourself if you are not
    // using that function (i.e. custom particle rendering)
    void setUniforms(ofShader& shader);
    
    void setBorderTexture(ofTexture & tex){
        this->borderTex = &tex;
        borderTex->bind(14);
    }
    void setNoiseTexture(ofTexture & tex){
        this->noiseTex = &tex;
        noiseTex->bind(13);
    }
    
private:
    ofFbo fbos[2];
    ofVboMesh mesh;
    ofVboMesh quadMesh;
    ofxAutoReloadedShader updateShader, drawShader;
    unsigned currentReadFbo;
    unsigned textureLocation;
    unsigned width, height, numFloats;
    ofImage sparkImg;
    int     imgWidth, imgHeight;
    ofFbo dotFbo;
    ofxAutoReloadedShader dotShader;
    ofVboMesh quadMeshDot;
    
    ofTexture * borderTex;
    ofTexture * noiseTex;

};

