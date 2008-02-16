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
    return false;
}

const Intersection RegularGrid::getFirstIntersection(const RaySegment& ray) const {
  Intersection closest(Intersection::getEmpty());
// Fast Voxel Traversal by Amanatides and Woo
// :: Initialization phase ::
//    - Identify voxel in which ray origins
  int currentVox[3];
  RaySegment clippedRay(ray);
  const AABB& aabb = scene.getAABB();
  if ( !clippedRay.trim(aabb) )
    return Intersection::getEmpty();
  if ( clippedRay.getTMin() > 0.0f)  {
    // ray origin lies outside grid => calc first intersecting voxel
    const Vec3 entrancePoint(ray.getOrigin() + (clippedRay.getTMin()) * ray.getDirection());
    clippedRay.setOrigin(entrancePoint);
    clippedRay.setTMax(clippedRay.getTMax() - clippedRay.getTMin());
    clippedRay.setTMin(0.0f);
  }
  const Vec3& rayOrigin = clippedRay.getOrigin();
  for (unsigned int i = 0; i < 3; ++i)
    currentVox[i] = ( int ) ( ( rayOrigin[i] - aabb.getMin(i) ) * cellsizeInvert[i] ) ;

  int step[3]; // step to next voxel in direction of ray ( -1 or 1 )
  for (unsigned int i = 0; i < 3; ++i)
  step[i] = ( clippedRay.getDirection()[i] > 0 ) ? 1 : -1;

// :: Incremental phase ::
  unsigned int gridIdx = currentVox[2] * slabSize // slabs in z-direction
                         + currentVox[1] * resolution // + y-count rows
                         + currentVox[0];

  float tMax[3]; // these values indicate how much we can travel through the current Voxel
  //  into direction of one component until we hit the next voxel in that direction.
  // (in units of the direction vector of r)
  std::vector<int>::const_iterator iter;
  unsigned char component, i;
  float smallestT;
  Vec3 position( clippedRay.getOrigin() );

  // save maximal t before 'falling out of the grid'
  while ( closest.isEmpty() && (clippedRay.getTMin() < clippedRay.getTMax()) ) {
    gridIdx =     currentVox[2] * slabSize // slabs in z-direction
                  + currentVox[1] * resolution // + y-count rows
                  + currentVox[0];
    assert( gridIdx < (resolution * resolution * resolution));
    for ( i = 0; i < 3; ++i )
      tMax[i] = (( aabb.getMin(i) + ( currentVox[i] + step[i] ) * cellsize[i] ) - position[i] ) * ray.getInvDirection()[i];

    // determine the closest voxel and axis on which it will follow
    smallestT = tMax[0];
    component = 0;
    for ( i = 1; i < 3; ++i)
      if ( tMax[i] < smallestT ) {
        smallestT = tMax[i];
        component = i;
      }

    //  clippedRay.setTMax(  clippedRay.getTMin() + smallestT  );
    if ( grid[gridIdx] ) {
      for ( iter =  grid[gridIdx]->begin() ; iter != grid[gridIdx]->end() ; ++iter ) {
        const Primitive& currPrim = scene.getPrimitive(*iter);
        const Intersection current = currPrim.getIntersection( clippedRay );
        if ( current < closest)
          closest = current;
      }
    }

      // proceed to next voxel
      currentVox[component] += step[component];
      position += clippedRay.getDirection() * smallestT;
      clippedRay.setTMin( clippedRay.getTMin() + smallestT);
  } // end of incremental phase of voxel traversal
  return closest;
}

Float4 RegularGrid::haveIntersections(const RaySegmentSSE& ray) const {
}

void RegularGrid::construct() {
    // calculate size of a cell, in each dimension
    AABB aabb(scene.getAABB());
    aabb.scale(1.00001f);
    for ( unsigned int i = 0; i < 3; ++i) {
        cellsize[i] = (aabb.getMax(i) - aabb.getMin(i)) / resolution;
        cellsizeInvert[i] = 1.0 / cellsize[i];
    }

    // allocate mem for pointers to prim lists for each cell
    const unsigned int gridsize = resolution * resolution * resolution;
    grid = new std::vector<int>*[gridsize];
    memset ( grid, 0, gridsize * sizeof ( std::vector<int>* ) );
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
            const unsigned int lower = floorf((primAABB.getMin(c) - aabb.getMin(c)) * cellsizeInvert[c]);
            overlapInterval[2 * c] = lower;
            // determine last cell primitive overlaps ( in i-th dimension )
            const unsigned int upper = ceilf( ( primAABB.getMax(c) - aabb.getMin(c) ) * cellsizeInvert[c]);
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
                            grid[cellIdx] = new std::vector<int>;
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


