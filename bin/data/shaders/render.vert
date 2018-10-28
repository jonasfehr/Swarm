#version 330

uniform mat4 modelViewMatrix;

in vec2 texcoord;
in vec4 color;

uniform sampler2DRect particles0;
uniform sampler2DRect particles1;
uniform vec2 screen;
uniform vec2 inputSize;
uniform float numBoids;
//
out vec4 vPosition;
out vec2 vTexCoord;
// out vec4 vColor;
vec2 indexToTexCoord(int index){
  vec2 texCoord;
  texCoord.x = mod(index, inputSize.x);
  texCoord.y = (index-texCoord.x)/inputSize.x;
  return texCoord;
}

int texCoordToIndex(vec2 texCoord){
  int index;
  index = int(texCoord.y*inputSize.x+texCoord.x);
  return index;
}

void main() {
  int index = texCoordToIndex(texcoord);

  if(index > int(numBoids*inputSize.x*inputSize.y)) return;

    // Read position data from texture.
    vec4 pixPos = texture( particles0, texcoord );

    // Map the position from the texture (from 0.0 to 1.0) to
    // the screen position (0 - screenWidth/screenHeight)
    pixPos = pixPos.xzyw;
    // pixPos.z = 0.0;
    pixPos.w = 0.1;
    // pixPos.x *= screen.x;
    // pixPos.z *= screen.y;

    pixPos.x *=12.8;
    pixPos.y *=12.8;
    pixPos.z *=12.8;

    vPosition = pixPos;
    vTexCoord = texcoord;
    // vColor = color;
}
