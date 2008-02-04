//
// C++ Interface: accelerationstruct
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ACCELERATIONSTRUCT_H
#define ACCELERATIONSTRUCT_H

#include "triangle.h"
#include "rgbvalue.h"
#include "scene.h"
#include "ray.h"
class RayPacket;
class Ray4;
/**
Superclass for all classes that implement acceleration data structures (Regular grid, BIH-tree etc)

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class AccelerationStruct {
public:
    // methods to be overwritten by deriving classes

    AccelerationStruct(const Scene& scene);
    virtual ~AccelerationStruct();
    virtual const RGBvalue trace(RadianceRay& r, unsigned int depth = 5) const = 0;
    virtual bool trace( RayPacket& rp, unsigned int depth = 5 ) const { return false;};
    virtual void findAllIntersections(std::vector<Intersection> results, RadianceRay& r) {;}
    virtual const Intersection& getClosestIntersection(RadianceRay& r) const = 0;
    /**
      @return if the given Ray is blocked between tmin and tmax
    **/
    virtual bool isBlocked(Ray& r) const = 0;
    virtual void construct() = 0;

    // common methods used by all acceleration structures

    unsigned int getTriangleCount() const { return triangles.size(); }
    void setBounds(float* newBounds);
    const Triangle& getTriangle(unsigned int idx) const { assert(idx < triangles.size());return triangles[idx]; }
    Triangle& getTriangle(unsigned int idx) { return triangles[idx]; }
    bool trimRaytoBounds(Ray &ray) const;
    bool trimRaytoBounds(RayPacket &raypacket) const;
    bool trimRaytoBounds(Ray4 &r4) const;
    void addTriangle(const Triangle& t);
    void addVertex(const Vector3D& v) {vertices.push_back(v);}
    const Vector3D& getVertex(const unsigned int i) const {assert(i < vertices.size());  return vertices[i]; }
    Vector3D& getVertex(const unsigned int i) {assert(i < vertices.size()); return vertices[i]; }
    const unsigned int getVertexCount() const { return vertices.size(); }
    virtual void analyze() const { std::cout << "not implemented." << std::endl ;}

protected:
    std::vector<Triangle> triangles;
    std::vector<Vector3D> vertices;
    float bounds[6]; //xmin,xmax,ymin,ymax,zmin,zmax
    bool boundsSet; // becomes true when setBounds is called
    const Scene& scene;
};

#endif
