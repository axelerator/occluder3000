//
// C++ Interface: regulargrid
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERREGULARGRID_H
#define OCCLUDERREGULARGRID_H

#include <accelerationstructure.h>

namespace Occluder {

/**
Primitives are sorted int the cells of a regular grid.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RegularGrid : public AccelerationStructure
{
public:
    RegularGrid(const unsigned int resolution, const Scene& scene);

    ~RegularGrid();

    virtual bool hasIntersection(const RaySegment& ray) const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
    virtual Float4 haveIntersections(const RaySegmentSSE& ray) const;
    virtual void construct();
    virtual void determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const;
    virtual void getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const;
private:
    std::vector<int> **grid;
    float cellsize[3]; // edgelength of one gridcell (till now always cubes)
    float cellsizeInvert[3]; // 1/cellsize
    const unsigned int resolution;
    const unsigned int slabSize; // basically a cache for how many cells are in one plane
    std::vector<int> emptyList;
};

}

#endif
