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

/**
  * A Ray Chunk represents a set of rays that can be processed
  * in parallel with SSE instruction
**/
class RayChunk {
  public:
  
  private:
    float origin[3] __attribute__((aligned(16)));
    float direction[3] __attribute__((aligned(16)));
    float tmin[3] __attribute__((aligned(16)));
    float tmax[3] __attribute__((aligned(16)));
};


#define PACKET_WIDTH 8
#define RAY_COUNT PACKET_WIDTH*PACKET_WIDTH


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
    const Vector3D& getInvDirection(unsigned int i)  const { return invDirection[i]; }
    const Vector3D& getDirection(unsigned int i)  const { return direction[i]; }
    const Vector3D& getOrigin() const { return origin; }
    const float getMin(unsigned int i) const { return tmin[i]; }
    const float getMax(unsigned int i) const { return tmax[i]; }
    const RGBvalue& getColor(unsigned int i) { return col[i]; }
    
    // not const methods
    /**
      @return true if signs of all components equal
    **/
    bool set(const Vector3D& origin, const Vector3D& u, const Vector3D& v, const Vector3D& packetu);
    void setMiss(unsigned int i, bool val) { missMask[i] = val;}
    void setMin(unsigned int i, float val) { tmin[i] = val; }
    void setMax(unsigned int i, float val) { tmax[i] = val; }
    void shade( unsigned int depth = 5);
    Intersection& getClosestIntersection(unsigned int i) { return closestIntersection[i]; }
    ~RayPacket();
  
    // static
    Vector3D refractRay(const Vector3D& e, const Vector3D& n, float nFrom, float nTo);
  
  private:
    Vector3D origin; // assuming all rays have same origin
    Vector3D direction[RAY_COUNT];
    Vector3D invDirection[RAY_COUNT];
    float tmin[RAY_COUNT];
    float tmax[RAY_COUNT];
    bool missMask[RAY_COUNT];
    const Scene& scene;
    RGBvalue col[RAY_COUNT];
    Intersection closestIntersection[RAY_COUNT];
};

#endif
