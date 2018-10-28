#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main()
{
    ofGLFWWindowSettings settings;
    settings.setGLVersion(3, 3);
    settings.setSize(1024, 1024);
    ofCreateWindow(settings);

    ofRunApp(new ofApp());
}
