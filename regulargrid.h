//
// C++ Interface: regulargrid
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef REGULARGRID_H
#define REGULARGRID_H

#include <vector>
#include "accelerationstruct.h"
#include "box.h"

/**
Partitiones space into a raster of identically sized axis-aligned boxes

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RegularGrid : public AccelerationStruct {
public:
    RegularGrid(const Scene& scene, float cellsize);
    virtual ~RegularGrid();
    
    virtual const RGBvalue trace(Ray& r, unsigned int depth = 0);
    virtual void construct();
//     virtual void drawGL();
private:
    bool findIntersection(Ray& r, IntersectionResult& ir);
    bool hasIntersection(Ray& r,  int ingoreTriangle);

    std::vector<int> **grid;
    float cellsize; // edgelength of one gridcell (till now always cubes)
    float cellsizeInvert; // 1/cellsize
    unsigned int cellCount[3];
    Box gridBox; // the boundingbox of the whole grid
    unsigned int slabSize; // amount of boxes in 'one x-y plane'

    
};

#endif
