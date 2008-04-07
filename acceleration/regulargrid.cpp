//
// C++ Implementation: regulargrid
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "regulargrid.h"
#include <iostream>
#include <assert.h>
#include "scene.h"

using namespace Occluder ;

RegularGrid::RegularGrid(const unsigned int resolution, const Scene& scene)
: AccelerationStructure(scene), resolution(resolution), slabSize(resolution * resolution) {

}


RegularGrid::~RegularGrid() {
    const unsigned int gridsize = resolution * resolution * resolution;
    for ( unsigned int i = 0; i < gridsize; ++i )
        if ( grid[i]->size() > 0 )
            delete grid[i];
    delete[] grid;
}

bool RegularGrid::hasIntersection(const RaySegment& ray) const {
    return !getFirstIntersection(ray).isEmpty();
}

const Intersection RegularGrid::getFirstIntersection(const RaySegment& ray) const {
  Intersection closest(Intersection::getEmpty());

  RaySegment clippedRay(ray);
  const AABB& aabb = scene.getAABB();

  if ( !clippedRay.trim(aabb) ) 
    return Intersection::getEmpty();

  if ( clippedRay.getTMin() > 0.0f)
    clippedRay.setOrigin(clippedRay.getOrigin() + clippedRay.getDirection() * clippedRay.getTMin());
  clippedRay.setTMin(0.0);

  unsigned int currentVoxelIndex[3];
  for (unsigned int i = 0; i < 3; ++i) {
    int idx = ( int ) floorf( ( clippedRay.getOrigin()[i] - aabb.getMin(i) ) * cellsizeInvert[i] );
    currentVoxelIndex[i] = ( idx < 0 ) ? 0 : idx;
    currentVoxelIndex[i] = ( currentVoxelIndex[i] >= resolution ) ? ( resolution - 1 ) : currentVoxelIndex[i];
  }

  int step[3]; // step to next voxel in direction of ray ( -1 or 1 )
  for (unsigned int i = 0; i < 3; ++i)
    step[i] = ( clippedRay.getDirection()[i] > 0 ) ? 1 : -1;

  List<int>::const_iterator iter;
  unsigned char i, nextStep;
  float tNext[3];
  unsigned int gridIdx = currentVoxelIndex[2] * slabSize + currentVoxelIndex[1] * resolution + currentVoxelIndex[0];
  do  {
    for ( i = 0; i < 3; ++i ) 
      tNext[i] = ((aabb.getMin(i) + (currentVoxelIndex[i] + (step[i] > 0)) * cellsize[i])
                 - clippedRay.getOrigin()[i]) * clippedRay.getInvDirection()[i];
    nextStep = 0;
    i = 0;
    if ( tNext[1] < tNext[0] ) 
      nextStep = 1;
    if ( tNext[2] < tNext[nextStep] ) 
      nextStep = 2;

    clippedRay.setTMax(tNext[nextStep]);
    for ( iter =  grid[gridIdx]->begin() ; iter != grid[gridIdx]->end() ; ++iter ) 
        closest += scene.getPrimitive(*iter).getIntersection( clippedRay );

    currentVoxelIndex[nextStep] += step[nextStep];
    clippedRay.setOrigin(clippedRay.getOrigin() + tNext[nextStep] * clippedRay.getDirection());
    gridIdx = currentVoxelIndex[2] * slabSize + currentVoxelIndex[1] * resolution + currentVoxelIndex[0];
  } while ( (closest.isEmpty() ) && (currentVoxelIndex[nextStep] < resolution) );
  return closest;
}



Float4 RegularGrid::haveIntersections(const RaySegmentSSE& ray) const {
  return 0.0f;
}

void RegularGrid::construct() {
    // calculate size of a cell, in each dimension
    const AABB& aabb(scene.getAABB());
    for ( unsigned int i = 0; i < 3; ++i) {
        cellsize[i] = (aabb.getMax(i) - aabb.getMin(i)) / resolution ;
        cellsizeInvert[i] = 1.0 / cellsize[i];
    }

    // allocate mem for pointers to prim lists for each cell
    const unsigned int gridsize = resolution * resolution * resolution;
    grid = new std::vector<int>*[gridsize];
    memset ( grid, 0, gridsize * sizeof ( List<int>* ) );
    for ( unsigned int g = 0; g < gridsize; ++g ) {
      grid[g] = &emptyList;
    }
    // sort  prims into  overlapping cells
    const unsigned int primCount = scene.getPrimitiveCount();
    unsigned int overlapInterval[6];
    for ( unsigned int i = 0; i < primCount; ++i) {
        const Primitive& primitive = scene.getPrimitive(i);
        const AABB primAABB( primitive.getAABB());
        for ( unsigned int c = 0; c < 3; ++c) {
            // determine first cell primitive overlaps ( in i-th dimension )
            const unsigned int lower = (unsigned int)floorf((primAABB.getMin(c) - aabb.getMin(c)) * cellsizeInvert[c]);
            overlapInterval[2 * c] = lower;
            // determine last cell primitive overlaps ( in i-th dimension )
            const unsigned int upper = (unsigned int)ceilf( ( primAABB.getMax(c) - aabb.getMin(c) ) * cellsizeInvert[c]);
            overlapInterval[2 * c + 1] = ( upper < ( lower + 1) ) ? lower + 1: upper ;


        }
        // loop over all cells that intersect aabb of primitive
        // and test for actual intersection with the primitive
        for ( unsigned int z = overlapInterval[4]; z < overlapInterval[5]; ++z)
            for ( unsigned int y = overlapInterval[2]; y < overlapInterval[3]; ++y)
                for ( unsigned int x = overlapInterval[0]; x < overlapInterval[1]; ++x) {
                    const Vec3 min(aabb.getMin() + Vec3( x * cellsize[0],  y * cellsize[1],  z * cellsize[2] ));
                    const Vec3 max(min + Vec3( cellsize[0],  cellsize[1], cellsize[2] ));
                    const AABB currGridCell(min, max);
                    if ( currGridCell.intersects( primitive ) ) {
                        const unsigned int cellIdx = z * slabSize + y * resolution + x;
                        if ( grid[cellIdx]->size() == 0 )
                            grid[cellIdx] = new List<int>;
                        grid[cellIdx]->push_back(i);
                    }
                }
    }

    for ( unsigned int z = 0; z < resolution; ++z) {
        for ( unsigned int y = 0; y < resolution; ++y) {
            for ( unsigned int x = 0; x < resolution; ++x) {
                const unsigned int cellIdx = z * slabSize + y * resolution + x;
                std::cout << "\t" << ((grid[cellIdx])?grid[cellIdx]->size():0);
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void RegularGrid::determineFirstIntersection(const RaySegmentSSE& ray, IntersectionSSE& result) const {
}

void RegularGrid::getAllIntersections(const RaySegment& ray, List< const Intersection >& results) const {
}


