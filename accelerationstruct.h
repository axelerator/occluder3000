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
/**
Superclass for all classes that implement acceleration data structures (Regular grid, BIH-tree etc)

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class AccelerationStruct {
public:
    AccelerationStruct(const Scene& scene);
    virtual ~AccelerationStruct();
    void addTriangle(const Triangle& t);
    virtual const RGBvalue trace(RadianceRay& r, unsigned int depth = 5) = 0;
    virtual const Intersection& getClosestIntersection(RadianceRay& r) { return *((const Intersection*)(0));}
    virtual bool isBlocked(Ray& r) = 0;
    unsigned int getTriangleCount() const { return triangles.size(); }
    void setBounds(float* newBounds);
    virtual void construct() = 0;
    const Triangle& getTriangle(unsigned int idx) const { return triangles[idx]; }
    bool trimRaytoBounds(Ray &ray);
protected:
    std::vector<Triangle> triangles;
    float bounds[6]; //xmin,xmax,ymin,ymax,zmin,zmax
    bool boundsSet; // becomes true when setBounds is called
    const Scene& scene;
};

#endif
