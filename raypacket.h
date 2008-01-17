//
// C++ Interface: raypacket
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RAYPACKET_H
#define RAYPACKET_H
#include "radianceray.h"
#include "sse4.h"
#include <GL/gl.h>
typedef union {
  __m64 sse;
  unsigned short int i[4];
} usint4;

/**
  * A Ray4 represents a set of rays that can be processed
  * in parallel with SSE instruction
**/
class Ray4 {
  public:
    Ray4(){}
    Ray4(const Vector3D& origin, const Vector3D& d0, const Vector3D u, const Vector3D v, const AccelerationStruct *ac): mask(0), geometry(ac) {
      this->origin.c[0].v.sse = _mm_set_ps1( origin.value[0] );
      this->origin.c[1].v.sse = _mm_set_ps1( origin.value[1] );
      this->origin.c[2].v.sse = _mm_set_ps1( origin.value[2] );
      
      direction.c[0].v.sse = _mm_set_ps1( d0.value[0] );
      direction.c[1].v.sse = _mm_set_ps1( d0.value[1] );
      direction.c[2].v.sse = _mm_set_ps1( d0.value[2] );
      
      direction.c[0].v.sse = _mm_add_ps(direction.c[0].v.sse , _mm_set_ps(u.value[0] + v.value[0], v.value[0], u.value[0], 0.0 ));
      direction.c[1].v.sse = _mm_add_ps(direction.c[1].v.sse , _mm_set_ps(u.value[1] + v.value[1], v.value[1], u.value[1], 0.0));
      direction.c[2].v.sse = _mm_add_ps(direction.c[2].v.sse , _mm_set_ps(u.value[2] + v.value[2], v.value[2], u.value[2],0.0)); 
      
      tmin.v.sse = _mm_setzero_ps();
      tmax.v.sse = _mm_set1_ps(100000.0);
      t = tmax;
    }
    
    
    void set(const Vector3D& origin, const Vector3D& d0, const Vector3D u, const Vector3D v, const AccelerationStruct *ac) {
      this->origin.c[0].v.sse = _mm_set_ps1( origin.value[0] );
      this->origin.c[1].v.sse = _mm_set_ps1( origin.value[1] );
      this->origin.c[2].v.sse = _mm_set_ps1( origin.value[2] );
      
      direction.c[0].v.sse = _mm_set_ps1( d0.value[0] );
      direction.c[1].v.sse = _mm_set_ps1( d0.value[1] );
      direction.c[2].v.sse = _mm_set_ps1( d0.value[2] );
      
      direction.c[0].v.sse = _mm_add_ps(direction.c[0].v.sse , _mm_set_ps(u.value[0] + v.value[0], v.value[0], u.value[0], 0.0 ));
      direction.c[1].v.sse = _mm_add_ps(direction.c[1].v.sse , _mm_set_ps(u.value[1] + v.value[1], v.value[1], u.value[1], 0.0));
      direction.c[2].v.sse = _mm_add_ps(direction.c[2].v.sse , _mm_set_ps(u.value[2] + v.value[2], v.value[2], u.value[2], 0.0));  
      
      inv_direction.c[0].v.sse = _mm_div_ps(ONE, direction.c[0].v.sse);
      inv_direction.c[1].v.sse = _mm_div_ps(ONE, direction.c[1].v.sse);
      inv_direction.c[2].v.sse = _mm_div_ps(ONE, direction.c[2].v.sse);
      
      tmin.v.sse = _mm_setzero_ps();
      tmax.v.sse = _mm_set1_ps(100000.0);
      t = tmax;
      mask = dead = 0;
      this->geometry = ac;
    }
       
    SSEVec3D origin;
    SSEVec3D direction;
    SSE4 tmin;
    SSE4 tmax;
    SSE4 u,v,t;
    int mask;
    SSE4 hitTriangle;
    const AccelerationStruct *geometry;
    SSEVec3D inv_direction;
    int dead;
};

#define PACKET_WIDTH 4
#define RAY_COUNT (PACKET_WIDTH*PACKET_WIDTH)


/**
 * A packet of coherent rays which will be traced together through
 * the accelleration structure.
*/
class RayPacket {
  public:
    RayPacket(const Scene& scene, const Vector3D& eye);
    // const methods
    
    static unsigned int getPacketWidth() { return PACKET_WIDTH; }
    static unsigned int getRayCount() { return RAY_COUNT; }
    static unsigned int getR4Count() { return RAY_COUNT/4; }
    const Vector3D& getOrigin() const { return origin; }
    
    // not const methods
    /**
      @return true if signs of all components equal
    **/
    bool set(const Vector3D& origin, const Vector3D& u, const Vector3D& v, const Vector3D& packetu, const Vector3D& packetv );
    void shade( GLubyte *fbuffer, unsigned int stride, unsigned int depth = 5);
    ~RayPacket();
  
    // static
    Vector3D refractRay(const Vector3D& e, const Vector3D& n, float nFrom, float nTo);
  
    Ray4 r4[RAY_COUNT / 4];


    Vector3D origin; // assuming all rays have same origin
    const Scene& scene;
    SSEVec3D rgb[RAY_COUNT];
    Ray4 shaft;
};

#endif
