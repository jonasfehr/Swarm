#version 330

// This fill the billboard made on the Geometry Shader with a texture
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;
uniform sampler2DRect sparkTex;
uniform float numBoids;
uniform vec2 inputSize;

in vec2 vTexCoord;

out vec4 vFragColor;

int texCoordToIndex(vec2 texCoord){
  int index;
  index = int(texCoord.y*inputSize.x+texCoord.x);
  return index;
}

void main() {

  int index = texCoordToIndex(vTexCoord);

  if ( index >= int(numBoids*inputSize.x*inputSize.y) )
  {
    vec3 col = texture(sparkTex, vTexCoord).rgb;
    vFragColor = vec4(col, 0.299*col.r+0.587*col.b+0.144*col.g);
  }else {
    vFragColor = vec4(0);
  }


}
