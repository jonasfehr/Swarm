/*{
  "DESCRIPTION": "aids when mapping 3D fixturres",
  "CREDIT": "Jonas Fehr (jonasfehr.ch)",
  "CATEGORIES": [
    "Generator"
  ],
  "INPUTS" : [
    { "NAME" : "size", "TYPE" : "float", "MAX" : 1, "DEFAULT" : 1, "MIN" : 0, "LABEL" : "size" },
    { "NAME" : "feather", "TYPE" : "float", "MAX" : 1, "DEFAULT" : 0, "MIN" : 0, "LABEL" : "feather" },
    { "NAME" : "gamma", "TYPE" : "float", "MAX" : 2, "DEFAULT" :1, "MIN" : 0.000001, "LABEL" : "gamma" }
    ]
}*/

#version 330

uniform vec2 RENDERSIZE;
uniform float size;
uniform float feather;
uniform float gamma;
uniform float transparency;

in vec2 texCoordVarying;
out vec4 fragColor;

float mapNum(float s, float a1, float a2, float b1, float b2)
{
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}


void main(  ){
    vec2 st = texCoordVarying.xy / RENDERSIZE.xy;

    float dist = 1.-distance(st, vec2(0.5));

    float start = mapNum(size, 1., 0., 0.5, 1.);
    float end = mapNum(feather, 1., 0., 1., start);

    dist = smoothstep(start,end, dist);

    dist = pow(dist, gamma<=1? gamma : mapNum(gamma, 1., 2., 1., 16.));


    // float dsitX = smoothstep(1.-posX-sizeX,1.-posX-sizeX+feaderX*sizeX, 1.-st.x)*smoothstep(posX-sizeX,posX-sizeX+feaderX*sizeX, st.x);
    // float dsitY = smoothstep(1.-posY-sizeY,1.-posY-sizeY+feaderY*sizeY, 1.-st.y)*smoothstep(posY-sizeY,posY-sizeY+feaderY*sizeY, st.y);



    // float val;
    // if(!invert) val = (dsitX*dsitY);
    // else        val = (1.-(dsitX*dsitY));

    // vec4 xyPlane = IMG_NORM_PIXEL(xyPlane_front, p.xy);
    // vec4 yzPlane = IMG_NORM_PIXEL(yzPlane_side, p.yz);
    // vec4 zxPlane = IMG_NORM_PIXEL(zxPlane_top, p.zx);
    //
    // vec4 color = xyPlane * yzPlane * zxPlane;



    fragColor = vec4( (dist*transparency));

}
