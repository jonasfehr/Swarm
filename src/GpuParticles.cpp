/*
 *  GpuParticles.cpp
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
#include "GpuParticles.h"

const string GpuParticles::UNIFORM_PREFIX = "particles";
const string GpuParticles::UPDATE_SHADER_NAME = "update";
const string GpuParticles::DRAW_SHADER_NAME = "draw";

GpuParticles::GpuParticles() : currentReadFbo(0), textureLocation(0)
{
}

void GpuParticles::init(unsigned width, unsigned height, ofPrimitiveMode primitive, bool loadDefaultShaders, unsigned numDataTextures)
{
    this->width = width;
    this->height = height;
    numFloats = width * height * FLOATS_PER_TEXEL;
    
    // fbos
    ofFbo::Settings s;
    s.internalformat = GL_RGBA32F_ARB;
    s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    s.minFilter = GL_NEAREST;
    s.maxFilter = GL_NEAREST;
    s.wrapModeHorizontal = GL_CLAMP;
    s.wrapModeVertical = GL_CLAMP;
    s.width = width;
    s.height = height;
    s.numColorbuffers = numDataTextures;
    for (unsigned i = 0; i < 2; ++i)
    {
        fbos[i].allocate(s);
    }
    
    // mesh
    mesh.clear();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            mesh.addVertex(ofVec3f(200.f * x / (float)width - 100.f, 200.f * y / (float)height - 100.f, -500.f));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    mesh.setMode(primitive);
    
    quadMesh.addVertex(ofVec3f(-1.f, -1.f, 0.f));
    quadMesh.addVertex(ofVec3f(1.f, -1.f, 0.f));
    quadMesh.addVertex(ofVec3f(1.f, 1.f, 0.f));
    quadMesh.addVertex(ofVec3f(-1.f, 1.f, 0.f));
    
    quadMesh.addTexCoord(ofVec2f(0.f, 0.f));
    quadMesh.addTexCoord(ofVec2f(width, 0.f));
    quadMesh.addTexCoord(ofVec2f(width, height));
    quadMesh.addTexCoord(ofVec2f(0.f, height));
    
    quadMesh.addIndex(0);
    quadMesh.addIndex(1);
    quadMesh.addIndex(2);
    quadMesh.addIndex(0);
    quadMesh.addIndex(2);
    quadMesh.addIndex(3);
    
    quadMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    
    // shaders
    if (loadDefaultShaders)
    {
        updateShader.load(UPDATE_SHADER_NAME);
        drawShader.load(DRAW_SHADER_NAME);
    }
    
    //        sparkImg.load("spark.png");
    //        sparkImg.getTexture().bind(15);
    //        sparkImg.update();
    
    dotFbo.allocate(64 , 64, GL_RGB32F);
    dotShader.load("shaders/Dot");
    dotFbo.getTexture().bind(15);
    
    quadMeshDot.addVertex(ofVec3f(-1.f, -1.f, 0.f));
    quadMeshDot.addVertex(ofVec3f(1.f, -1.f, 0.f));
    quadMeshDot.addVertex(ofVec3f(1.f, 1.f, 0.f));
    quadMeshDot.addVertex(ofVec3f(-1.f, 1.f, 0.f));
    
    quadMeshDot.addTexCoord(ofVec2f(0.f, 0.f));
    quadMeshDot.addTexCoord(ofVec2f(dotFbo.getWidth(), 0.f));
    quadMeshDot.addTexCoord(ofVec2f(dotFbo.getWidth(), dotFbo.getHeight()));
    quadMeshDot.addTexCoord(ofVec2f(0.f, dotFbo.getHeight()));
    
    quadMeshDot.addIndex(0);
    quadMeshDot.addIndex(1);
    quadMeshDot.addIndex(2);
    quadMeshDot.addIndex(0);
    quadMeshDot.addIndex(2);
    quadMeshDot.addIndex(3);
    
    quadMeshDot.setMode(OF_PRIMITIVE_TRIANGLES);
    
}

void GpuParticles::loadShaders(const string& updateShaderName, const string& drawShaderName)
{
    updateShader.load(updateShaderName);
    drawShader.load(drawShaderName);
}

void GpuParticles::update()
{
    fbos[1 - currentReadFbo].begin(OF_FBOMODE_NODEFAULTS);
    glPushAttrib(GL_ENABLE_BIT);
    // we set up no camera model and ignore the modelview and projection matrices
    // in the vertex shader, we make a viewport large enough to ensure the shader
    // is executed for each pixel
    glViewport(0, 0, width, height);
    glDisable(GL_BLEND);
    ofSetColor(255, 255, 255);
    fbos[1 - currentReadFbo].activateAllDrawBuffers();
    
    updateShader.begin();
    ofNotifyEvent(updateEvent, updateShader, this);
    setUniforms(updateShader);
    if(borderTex->isAllocated())updateShader.setUniformTexture("borderTex", *borderTex, 14);
    if(noiseTex->isAllocated())updateShader.setUniformTexture("noiseTex", *noiseTex, 13);
    updateShader.setUniform2f("inputSize", ofVec2f(width, height));
    updateShader.setUniform1i("iFrame", ofGetFrameNum());
    updateShader.setUniform1f("numBoids", numBoids);
    updateShader.setUniform1f("cohesion", (float)cohesion);
    updateShader.setUniform1f("separation", (float)separation);
    updateShader.setUniform1f("align", (float)align);
    updateShader.setUniform1f("borderAvoid", (float)borderAvoid);
    updateShader.setUniform1f("random", (float)random);
    updateShader.setUniform1f("noiseSteering", (float)noiseSteering);
    updateShader.setUniform1f("cohesionDist", (float)cohesionDist);
    updateShader.setUniform1f("separationDist", (float)separationDist);
    updateShader.setUniform1f("alignDist", (float)alignDist);
    updateShader.setUniform1i("do3D", (int)do2D);
    updateShader.setUniform1f("speed", (float)speedBoids);
    
    quadMesh.draw();
    updateShader.end();
    glPopAttrib();
    
    fbos[1 - currentReadFbo].end();
    
    currentReadFbo = 1 - currentReadFbo;
    
    dotFbo.begin();
    ofClear(0.);
    dotShader.begin();
    dotShader.setUniform2f("RENDERSIZE", dotFbo.getWidth(), dotFbo.getHeight());
    dotShader.setUniform1f("size", dotSize);
    dotShader.setUniform1f("feather", feather);
    dotShader.setUniform1f("gamma", gamma);
    dotShader.setUniform1f("transparency", transparency);
    quadMeshDot.draw();
    dotShader.end();
    dotFbo.end();
}

void GpuParticles::draw()
{
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    drawShader.begin();
    ofNotifyEvent(drawEvent, drawShader, this);
    setUniforms(drawShader);
    drawShader.setUniformTexture("sparkTex", dotFbo.getTexture() , 15);
    //    updateRender.setUniform1i("resolution", (float)textureRes);
    drawShader.setUniform2f("screen", (float)ofGetWidth(), (float)ofGetHeight());
    drawShader.setUniform1f("numBoids", numBoids);
    drawShader.setUniform1f("size", (float)pointScale*10.0);
    drawShader.setUniform2f("inputSize", ofVec2f(width, height));
    drawShader.setUniform1f("imgWidth", (float)dotFbo.getWidth());
    drawShader.setUniform1f("imgHeight", (float)dotFbo.getHeight());
    mesh.draw();
    drawShader.end();
    
    //        dotFbo.draw(0,0);
}

void GpuParticles::setUniforms(ofShader& shader)
{
    for (unsigned i = 0; i < fbos[currentReadFbo].getNumTextures(); ++i)
    {
        ostringstream oss;
        oss << UNIFORM_PREFIX << ofToString(i);
        shader.setUniformTexture(oss.str().c_str(), fbos[currentReadFbo].getTexture(i), i + textureLocation);
    }
    
}

void GpuParticles::loadDataTexture(unsigned idx, float* data,
                                   unsigned x, unsigned y, unsigned width, unsigned height)
{
    if (idx < fbos[currentReadFbo].getNumTextures())
    {
        if (!width) width = this->width;
        if (!height) height = this->height;
        fbos[currentReadFbo].getTexture(idx).bind();
        glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, x, y, width, height, GL_RGBA, GL_FLOAT, data);
        fbos[currentReadFbo].getTexture(idx).unbind();
    }
    else ofLogError() << "Trying to load data from array into non-existent buffer.";
}

void GpuParticles::zeroDataTexture(unsigned idx,
                                   unsigned x, unsigned y, unsigned width, unsigned height)
{
    if (!width) width = this->width;
    if (!height) height = this->height;
    float* zeroes = new float[width * height * FLOATS_PER_TEXEL];
    memset(zeroes, 0, sizeof(float) * width * height * FLOATS_PER_TEXEL);
    loadDataTexture(idx, zeroes, x, y, width, height);
    delete[] zeroes;
}

void GpuParticles::save(const string& fileName)
{
    ofstream fileStream(ofToDataPath(fileName, true).c_str());
    if (fileStream.is_open())
    {
        for (unsigned i = 0; i < fbos[currentReadFbo].getNumTextures(); ++i)
        {
            if (i) fileStream << "|";
            ofFloatPixels pixels;
            fbos[currentReadFbo].getTexture(i).readToPixels(pixels);
            for (unsigned j = 0; j < pixels.size(); ++j)
            {
                if (j) fileStream << ",";
                fileStream << pixels[j];
            }
        }
        fileStream.close();
    }
    else ofLogError() << "Could not save particle data to " << ofToDataPath(fileName, true);
}

void GpuParticles::load(const string& fileName)
{
    ifstream fileStream(ofToDataPath(fileName, true).c_str());
    if (fileStream.is_open())
    {
        string data((istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
        vector<string> textureData = ofSplitString(data, "|");
        for (unsigned i = 0; i < textureData.size(); ++i)
        {
            if (i < fbos[currentReadFbo].getNumTextures())
            {
                vector<string> floatsAsText = ofSplitString(textureData[i], ",");
                vector<float> floats(floatsAsText.size(), 0);
                for (unsigned j = 0; j < floats.size(); ++j)
                {
                    floats[j] = atof(floatsAsText[j].c_str());
                }
                loadDataTexture(i, &floats[0]);
            }
            else ofLogError() << "Trying to load data from file into non-existent buffer.";
        }
        fileStream.close();
    }
    else ofLogError() << "Could not load particle data from " << ofToDataPath(fileName, true);
}

