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
    virtual const RGBvalue trace(Ray& r, unsigned int depth = 0) = 0;
    unsigned int getTriangleCount() const { return triangles.size(); }
    void setBounds(fliess* newBounds);
    virtual void construct() = 0;

protected:
    std::vector<Triangle> triangles;
    fliess bounds[6]; //xmin,xmax,ymin,ymax,zmin,zmax
    bool boundsSet; // becomes true when setBounds is called
    const Scene& scene;
};

#endif
