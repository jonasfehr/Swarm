#version 330

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform vec3 mouse;
uniform float radiusSquared;
uniform float elapsed;
uniform vec2 size;
uniform int iFrame;

in vec2 texCoordVarying;

layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;

///////////////////////////
// Common Code
///////////////////////////

// -------------- 8< -------------- 8< -------------- 8< -------------- 8< -------------- 8< --------------

///////////////////////////
// Data Storage
///////////////////////////

vec4 LoadVec4( sampler2D sampler, in ivec2 vAddr )
{
    return texelFetch( sampler, vAddr, 0 );
}

vec3 LoadVec3( sampler2D sampler, in ivec2 vAddr )
{
    return LoadVec4( sampler, vAddr ).xyz;
}

bool AtAddress( ivec2 p, ivec2 c ) { return all( equal( p, c ) ); }

void StoreVec4( in ivec2 vAddr, in vec4 vValue, inout vec4 fragColor, in ivec2 fragCoord )
{
    fragColor = AtAddress( fragCoord, vAddr ) ? vValue : fragColor;
}

void StoreVec3( in ivec2 vAddr, in vec3 vValue, inout vec4 fragColor, in ivec2 fragCoord )
{
    StoreVec4( vAddr, vec4( vValue, 0.0 ), fragColor, fragCoord);
}

///////////////////////////
// Hash generation
///////////////////////////

// From Hash without Sine - by Dave Hoskins: https://www.shadertoy.com/view/4djSRW

#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)

//----------------------------------------------------------------------------------------
//  3 out, 1 in...
vec3 hash31(float p)
{
   vec3 p3 = fract(vec3(p) * HASHSCALE3);
   p3 += dot(p3, p3.yzx+19.19);
   return fract((p3.xxy+p3.yzz)*p3.zyx);
}


// -------------- 8< -------------- 8< -------------- 8< -------------- 8< -------------- 8< --------------

const int MAX_BOID_COUNT = 100;
const vec3 WORLD_SIZE = vec3( 10 );

float Scene_Distance( vec3 vPos )
{
    float fDist = 1000.0;

    float fInset = 0.3;

    // walls
    fDist = min( fDist, vPos.x - fInset );
    fDist = min( fDist, WORLD_SIZE.x - fInset - vPos.x );
    fDist = min( fDist, vPos.z - fInset );
    fDist = min( fDist, WORLD_SIZE.z - fInset - vPos.z );

    {
        vec3 vRockPos = vec3(4, 0, 2);
        float fRockSize = 1.0;
        float fRockDist = length( vPos - vRockPos ) - fRockSize;
        fDist = min( fDist, fRockDist );
    }

    {
        vec3 vRockPos = vec3(8, 0, 4);
        float fRockSize = 0.4;
        float fRockDist = length( vPos - vRockPos ) - fRockSize;
        fDist = min( fDist, fRockDist );
    }

    {
        vec3 vRockPos = vec3(3, 0, 6);
        float fRockSize = 1.5;
        float fRockDist = length( vPos - vRockPos ) - fRockSize;
        fDist = min( fDist, fRockDist );
    }

    return fDist + sin(vPos.x * 3.0) * 0.1 + sin(vPos.z * 3.0) * 0.1;
}

vec3 Scene_Normal( vec3 vPos )
{
    const float fDelta = 0.0001;
    vec2 e = vec2( -1, 1 );

    vec3 vNormal =
        Scene_Distance( e.yxx * fDelta + vPos ) * e.yxx +
        Scene_Distance( e.xxy * fDelta + vPos ) * e.xxy +
        Scene_Distance( e.xyx * fDelta + vPos ) * e.xyx +
        Scene_Distance( e.yyy * fDelta + vPos ) * e.yyy;

    return normalize( vNormal );
}

struct Boid
{
    vec3 vPos;
    vec3 vVel;

    vec3 vCohesionCentre; 	// only used for visualization
    vec3 vSeparationSteer;	// only used for visualization
    vec3 vAlignmentDir; 	// only used for visualization
};

vec2 indexToTexCoord(int index){
  vec2 texCoord;
  texCoord.x = mod(index, size.x);
  texCoord.y = index-texCoord.x*size.x;
  return texCoord;
}

int texCoordToIndex(vec2 texCoord){
  int index;
  index = int(texCoord.x*size.x+texCoord.y);
  return index;
}


Boid LoadBoid( int index )
{

    Boid boid;
    boid.vPos = texture(particles0, indexToTexCoord(index)).xyz;
    boid.vVel = texture(particles1, indexToTexCoord(index)).xyz;
    // boid.vPos = LoadVec3( iChannelSim, ivec2(index, 0) );
    // boid.vVel = LoadVec3( iChannelSim, ivec2(index, 1) );
    // boid.vCohesionCentre = LoadVec3( iChannelSim, ivec2(index, 2) );
    // boid.vSeparationSteer = LoadVec3( iChannelSim, ivec2(index, 3) );
    // boid.vAlignmentDir = LoadVec3( iChannelSim, ivec2(index, 4) );

    return boid;
}

void StoreBoid( int index, Boid boid, inout vec4 fragColor, ivec2 fragCoord )
{

    // StoreVec3( ivec2( index, 0), boid.vPos, fragColor, fragCoord );
    // StoreVec3( ivec2( index, 1), boid.vVel, fragColor, fragCoord );
    // StoreVec3( ivec2( index, 2), boid.vCohesionCentre, fragColor, fragCoord );
    // StoreVec3( ivec2( index, 3), boid.vSeparationSteer, fragColor, fragCoord );
    // StoreVec3( ivec2( index, 4), boid.vAlignmentDir, fragColor, fragCoord );
}

void ClampMagnitude( inout vec3 v, float fMinMag, float fMaxMag )
{
    float fMagnitude = clamp( length( v ), fMinMag, fMaxMag );
    v = normalize( v ) * fMagnitude;
}

void ResetBoid( int index, inout Boid thisBoid )
{
    vec3 vHash = hash31( float( index ) );
    thisBoid.vPos = vHash * 10.0;
    thisBoid.vPos.y = 0.0;
    thisBoid.vVel = ( hash31( float( index + 123 ) ) * 2.0 - 1.0 ) * 0.1;
    thisBoid.vVel.y = 0.0;
}

void UpdateBoid( int index, inout Boid thisBoid )
{
    vec3 vSeparationSteering = vec3(0);
    vec3 vCohesionSteering = vec3(0);
    vec3 vAlignmentSteering = vec3(0);
    vec3 vRandomSteering = vec3(0);
    vec3 vCollisionAvoidSteering = vec3(0);


    float fSeparationDist = 0.75;
    float fCohesionDist = 0.75;
    float fAlignmentDist = 0.75;
    float fCollisionAvoidDist = 1.0;

    float fCohesionWeight = 0.01 * 0.5;//UI_GetFloat( DATA_COHESION );
    float fSeparationWeight = 0.0002 * 0.8;//UI_GetFloat( DATA_SEPARATION );
    float fAlignmentWeight = 0.1 * 0.5;//UI_GetFloat( DATA_ALIGNMENT );
    float fRandomWalkWeight = 0.002;
    float fCollisionAvoidWeight = 0.001;

	float fMaxSteer = 0.01;

    float fMinSpeed = 0.03;
    float fMaxSpeed = 0.1;

    bool bRestrictTo2d = true;

    thisBoid.vCohesionCentre = vec3(0);
    float fCohesionCount = 0.0;

    thisBoid.vAlignmentDir = vec3(0);
    float fAlignmentCount = 0.0;

    float boidCount = 100; // NUM BOIDS
    for ( int iOtherIndex = 0; iOtherIndex < MAX_BOID_COUNT; iOtherIndex++ )
    {
        if ( iOtherIndex >= int(boidCount) )
        {
            break;
        }

        // Don't consider ourself as neighbor
        if ( index == iOtherIndex )
        {
            continue;
        }

        Boid otherBoid = LoadBoid( iOtherIndex );

        vec3 vToOther = otherBoid.vPos - thisBoid.vPos;

        // wrap world co-ordinates
        vToOther = mod( vToOther + WORLD_SIZE * .5, WORLD_SIZE) - WORLD_SIZE * .5;
        vec3 vOtherPos = thisBoid.vPos + vToOther;

        float fDistToOther = length( vToOther );
        vec3 vDirToOther = normalize(vToOther);

        if ( fDistToOther < fSeparationDist )
        {
            float fSeparationStrength = 1.0 / (fDistToOther * fDistToOther);
            vSeparationSteering += -vDirToOther * fSeparationStrength;
        }

        if ( fDistToOther < fCohesionDist )
        {
            thisBoid.vCohesionCentre += vOtherPos;
            fCohesionCount++;
        }

        if ( fDistToOther < fAlignmentDist )
        {
            thisBoid.vAlignmentDir += otherBoid.vVel;
            fAlignmentCount++;
        }
    }

    if ( fCohesionCount > 0.0 )
    {
    	thisBoid.vCohesionCentre = thisBoid.vCohesionCentre / fCohesionCount;
    	vCohesionSteering += thisBoid.vCohesionCentre - thisBoid.vPos;
    }

    if ( fAlignmentCount > 0.0 )
    {
    	thisBoid.vAlignmentDir = thisBoid.vAlignmentDir / fAlignmentCount;
    	vAlignmentSteering += thisBoid.vAlignmentDir - thisBoid.vVel;
    }

	vRandomSteering = ( hash31( float( index ) + elapsed ) * 2.0 - 1.0 );

    // if ( UI_GetBool(DATA_WALLS) && UI_GetFloat(DATA_PAGE_NO) >= 8.0 )
    // {
    //     float fSceneDistance = Scene_Distance( thisBoid.vPos );
    //     if ( fSceneDistance < fCollisionAvoidDist )
    //     {
    //         vec3 vNormal = Scene_Normal( thisBoid.vPos );
    //         float fDist = fSceneDistance/ fCollisionAvoidDist;
    //         fDist = max( fDist, 0.01);
    //         vCollisionAvoidSteering += vNormal / ( fDist );
    //     }
    // }

    vec3 vSteer = vec3( 0 );
    vSteer += vCohesionSteering * fCohesionWeight;
    vSteer += vSeparationSteering * fSeparationWeight;
    vSteer += vAlignmentSteering * fAlignmentWeight;
    vSteer += vRandomSteering * fRandomWalkWeight;
    vSteer += vCollisionAvoidSteering * fCollisionAvoidWeight;

    thisBoid.vSeparationSteer = vSeparationSteering;

    if ( bRestrictTo2d )
    {
		vSteer.y = 0.0;
    }

    ClampMagnitude( vSteer, 0.0, 0.01 );

    thisBoid.vVel += vSteer;

    if ( bRestrictTo2d )
    {
		thisBoid.vVel.y = 0.0;
    }

    ClampMagnitude( thisBoid.vVel, fMinSpeed, fMaxSpeed );

    // Move

    thisBoid.vPos += thisBoid.vVel;
    if ( bRestrictTo2d )
    {
		thisBoid.vPos.y = 0.0;
    }

    thisBoid.vPos = mod(  thisBoid.vPos, WORLD_SIZE );
}

void main()
{
    int index = texCoordToIndex(texCoordVarying.st);

    Boid thisBoid = LoadBoid( index );

    if ( iFrame == 0 )
    {
        ResetBoid( index, thisBoid );
    }
    else
    {
    	UpdateBoid( index, thisBoid );
    }

    // fragColor = vec4(0);

      posOut = vec4(thisBoid.vPos, 1.0);
      velOut = vec4(thisBoid.vVel, 0.0);
//     StoreBoid( index, thisBoid, fragColor, ivec2(fragCoord) );
 }

// void main()
// {
//     vec3 pos = texture(particles0, texCoordVarying.st).xyz;
//     vec3 vel = texture(particles1, texCoordVarying.st).xyz;
//
//     // mouse attraction
//     vec3 direction = mouse - pos.xyz;
//     float distSquared = dot(direction, direction);
//     float magnitude = 500.0 * (1.0 - distSquared / radiusSquared);
//     vec3 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);
//
//     // gravity
//     force += vec3(0.0, -0.5, 0.0);
//
//     // accelerate
//     vel += elapsed * force;
//
//     // bounce off the sides
//     vel.x *= step(abs(pos.x), 512.0) * 2.0 - 1.0;
//     vel.y *= step(abs(pos.y), 384.0) * 2.0 - 1.0;
//
//     // damping
//     vel *= 0.995;
//
//     // move
//     pos += elapsed * vel;
//
//     posOut = vec4(pos, 1.0);
//     velOut = vec4(vel, 0.0);
// }
