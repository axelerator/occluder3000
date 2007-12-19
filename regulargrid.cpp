//
// C++ Implementation: regulargrid
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "regulargrid.h"
#include "triaabb.h"
#include "radianceray.h"

RegularGrid::RegularGrid ( const Scene& scene, float cellsize ) :
    AccelerationStruct ( scene ), grid ( 0 ), cellsize ( cellsize ), cellsizeInvert ( 1.0/cellsize ) {}


RegularGrid::~RegularGrid() {
  const unsigned int gridsize = cellCount[0]*cellCount[1]*cellCount[2];
  if ( grid ) {
    for ( float xpos = bounds[0]; xpos < bounds[1] ; xpos += cellsize )
      for ( float ypos = bounds[2]; ypos < bounds[3] ; ypos += cellsize )
        for ( float zpos = bounds[4]; zpos < bounds[5] ; zpos += cellsize ) {
          const unsigned int cellIdx = ( int ) ( ( zpos - bounds[4] ) / cellsize ) * slabSize // slabs in z-direction
                                       + ( int ) ( ( ypos - bounds[2] ) / cellsize ) * cellCount[0] // + y-count rows
                                       + ( int ) ( ( xpos - bounds[0] ) / cellsize );
          if ( cellIdx > gridsize ) {
            std::cerr << "IndexOutOfBounds: Tried to access non existent RegularGrid cell\n";
          }
          if ( grid[cellIdx] != 0 )
            delete grid[cellIdx];
        }
    delete grid;
  }
}

const Intersection& RegularGrid::getClosestIntersection ( RadianceRay& r ) {
  const Triangle *hitTriangle = 0;

// Fast Voxel Traversal by Amanatides and Woo

// :: Initialization phase ::
//    - Identify voxel in which ray origins
  Vector3D position ( r.getStart() );
  unsigned int currentVox[3];
  const Vector3D& rs = r.getStart();
  if ( ( rs.value[0] < bounds[0] ) || ( rs.value[1] < bounds[2] ) || ( rs.value[2] < bounds[4] )
       || ( rs.value[0] > bounds[1] ) || ( rs.value[1] > bounds[3] ) || ( rs.value[2] > bounds[5] ) ) {
    // ray origin lies outside grid => calc first intersecting voxel
    BoxRayIntersect bri;
    gridBox.intersect ( r, 0.0f, 1000.0, bri );
    if ( bri.hit ) {
      currentVox[0] = ( int ) ( ( bri.poi.value[0] - bounds[0] ) * cellsizeInvert ) ;
      currentVox[1] = ( int ) ( ( bri.poi.value[1] - bounds[2] ) * cellsizeInvert ) ;
      currentVox[2] = ( int ) ( ( bri.poi.value[2] - bounds[4] ) * cellsizeInvert ) ;
      position = bri.poi;
    } else {
      return r.getClosestIntersection();
    }
  } else {
    //std::cout << currentVoxCandidate[0] <<  "  |  " << currentVoxCandidate[1] <<  "  |  " << currentVoxCandidate[2] <<  "\n";
    currentVox[0] = ( int ) ( ( r.getStart().value[0] - bounds[0] ) * cellsizeInvert ) ;
    currentVox[1] = ( int ) ( ( r.getStart().value[1] - bounds[2] ) * cellsizeInvert ) ;
    currentVox[2] = ( int ) ( ( r.getStart().value[2] - bounds[4] ) * cellsizeInvert ) ;
  }

  int step[3]; // step to next voxel in direction of ray ( -1 or 1 )
  step[0] = ( r.getDirection().value[0] > 0 ) ? 1 : -1;
  step[1] = ( r.getDirection().value[1] > 0 ) ? 1 : -1;
  step[2] = ( r.getDirection().value[2] > 0 ) ? 1 : -1;
// :: Incremental phase ::
  unsigned int gridIdx = currentVox[2] * slabSize // slabs in z-direction
                         + currentVox[1] * cellCount[0] // + y-count rows
                         + currentVox[0];
  float zBuffer = UNENDLICH;

  IntersectionResult closestRi;
  float tMax[3]; // these values indicate how much we can travel through the current Voxel
  //  into direction of one component until we hit the next voxel in that direction.
  // (in units of the direction vector of r)
  std::vector<int>::const_iterator iter;
  unsigned char component, i;
  float smallestT;
  while ( !hitTriangle && ( currentVox[0] < cellCount[0] )
          && ( currentVox[1] < cellCount[1] )  && ( currentVox[2] < cellCount[2] ) ) {
    gridIdx =     currentVox[2] * slabSize // slabs in z-direction
                  + currentVox[1] * cellCount[0] // + y-count rows
                  + currentVox[0];
    zBuffer = 10000.0;
    hitTriangle = 0;


    for ( i = 0; i < 3; ++i )
      tMax[i] = ( ( bounds[2*i] + ( currentVox[i] + ( step[i] > 0 ) ) * cellsize ) - position.value[i] ) / r.getDirection().value[i];

    smallestT = tMax[0];// determine the closest voxel and axis on which it will follow
    component = 0;
    if ( tMax[1] < smallestT ) {
      smallestT = tMax[1];
      component = 1;
    }
    if ( tMax[2] < smallestT ) {
      smallestT = tMax[2];
      component = 2;
    }

    if ( grid[gridIdx] ) {
      for ( iter =  grid[gridIdx]->begin() ; iter != grid[gridIdx]->end() ; ++iter ) {
        const Triangle& currTri = triangles[*iter];
        currTri.intersect ( r );
      }
    }

    if ( !hitTriangle ) {
      // proceed to next voxel
      currentVox[component] += step[component];
      position += r.getDirection() * smallestT;
      r.setMin ( r.getMin() + smallestT );
    }
  } // end of incremental phase of voxel traversal
  return r.getClosestIntersection();
}

bool RegularGrid::isBlocked ( Ray& r ) {
  IntersectionResult ir;
// Fast Voxel Traversal by Amanatides and Woo

// :: Initialization phase ::
//    - Identify voxel in which ray origins
  Vector3D position ( r.getStart() );
  unsigned int currentVox[3];
  const Vector3D& rs = r.getStart();
  if ( ( rs.value[0] < bounds[0] ) || ( rs.value[1] < bounds[2] ) || ( rs.value[2] < bounds[4] )
       || ( rs.value[0] > bounds[1] ) || ( rs.value[1] > bounds[3] ) || ( rs.value[2] > bounds[5] ) ) {
    // ray origin lies outside grid => calc first intersecting voxel
    BoxRayIntersect bri;
    gridBox.intersect ( r, 0.0f, 1000.0, bri );
    if ( bri.hit ) {
      currentVox[0] = ( int ) ( ( bri.poi.value[0] - bounds[0] ) * cellsizeInvert ) ;
      currentVox[1] = ( int ) ( ( bri.poi.value[1] - bounds[2] ) * cellsizeInvert ) ;
      currentVox[2] = ( int ) ( ( bri.poi.value[2] - bounds[4] ) * cellsizeInvert ) ;
      position = bri.poi;
    } else {
      return false;
    }
  } else {
    //std::cout << currentVoxCandidate[0] <<  "  |  " << currentVoxCandidate[1] <<  "  |  " << currentVoxCandidate[2] <<  "\n";
    currentVox[0] = ( int ) ( ( r.getStart().value[0] - bounds[0] ) * cellsizeInvert ) ;
    currentVox[1] = ( int ) ( ( r.getStart().value[1] - bounds[2] ) * cellsizeInvert ) ;
    currentVox[2] = ( int ) ( ( r.getStart().value[2] - bounds[4] ) * cellsizeInvert ) ;
  }

  int step[3]; // step to next voxel in direction of ray ( -1 or 1 )
  step[0] = ( r.getDirection().value[0] > 0 ) ? 1 : -1;
  step[1] = ( r.getDirection().value[1] > 0 ) ? 1 : -1;
  step[2] = ( r.getDirection().value[2] > 0 ) ? 1 : -1;
// :: Incremental phase ::
  unsigned int gridIdx = currentVox[2] * slabSize // slabs in z-direction
                         + currentVox[1] * cellCount[0] // + y-count rows
                         + currentVox[0];
  float tMax[3]; // these values indicate how much we can travel through the current Voxel
  //  into direction of one component until we hit the next voxel in that direction.
  // (in units of the direction vector of r)
  std::vector<int>::const_iterator iter;
  unsigned char i, component;
  while ( ( currentVox[0] < cellCount[0] )
          && ( currentVox[1] < cellCount[1] )  && ( currentVox[2] < cellCount[2] ) ) {
    gridIdx =     currentVox[2] * slabSize // slabs in z-direction
                  + currentVox[1] * cellCount[0] // + y-count rows
                  + currentVox[0];

    for ( i = 0; i < 3; ++i )
      tMax[i] = ( ( bounds[2*i] + ( currentVox[i] + ( step[i] > 0 ) ) * cellsize ) - position.value[i] ) / r.getDirection().value[i];

    float smallestT = tMax[0];// determine the closest voxel and axis on which it will follow
    component = 0;
    if ( tMax[1] < smallestT ) {
      smallestT = tMax[1];
      component = 1;
    }
    if ( tMax[2] < smallestT ) {
      smallestT = tMax[2];
      component = 2;
    }

    if ( grid[gridIdx] ) {
      for ( iter =  grid[gridIdx]->begin() ; iter != grid[gridIdx]->end() ; ++iter ) {
        const Triangle& currTri = triangles[*iter];
        if ( currTri.intersect ( r ) && ( r.getIgnored() != &currTri ) )
          return true;
      }
    }
    // proceed to next voxel
    currentVox[component] += step[component];
    position += r.getDirection() * smallestT;
    r.setMin ( r.getMin() + smallestT );

  } // end of incremental phase of voxel traversal
  return false;
}

const RGBvalue RegularGrid::trace ( RadianceRay& r, unsigned int depth ) {
  getClosestIntersection ( r );
  RGBvalue result ( 0.0, 0.0, 0.0 );
  r.shade ( result, depth );
  return result;
}

/**
  * Assisgns the triangles to the according gridcells
  * Setbounds has to be called first !!
  **/
void RegularGrid::construct() {
  if ( !boundsSet ) {
    std::cerr << "RegularGrid is tried to be constructed without setting bounds first !\n" ;
    return;
  }
  cellCount[0] = ( int ) ceilf ( ( bounds[1] - bounds[0] ) / cellsize );
  cellCount[1] = ( int ) ceilf ( ( bounds[3] - bounds[2] ) / cellsize );
  cellCount[2] = ( int ) ceilf ( ( bounds[5] - bounds[4] ) / cellsize );
  gridBox.parameters[0] = Vector3D ( bounds[0],bounds[2],bounds[4] );
  gridBox.parameters[1] = Vector3D ( bounds[1],bounds[3],bounds[5] );
  slabSize = cellCount[0] * cellCount[1];
  std::cout << "cells in x: " << cellCount[0] << " cells in y " << cellCount[1] << "  cells in z: " << cellCount[2] <<  "\n";

  // allocate a pointer to a vector of triangle indices for each cellsize
  const unsigned int gridsize = cellCount[0]*cellCount[1]*cellCount[2];
  grid = new std::vector<int>*[gridsize];
  memset ( grid, 0, gridsize * sizeof ( std::vector<int>* ) );
  std::vector<Triangle>::const_iterator iter;
  unsigned int component_min;
  unsigned int trianglesIdx = 0;
  const Vector3D boxhalfSize ( cellsize*0.5 );
  const float cellsizeHalf = cellsize * 0.5;
  for ( iter = triangles.begin(); iter != triangles.end() ; ++iter ) {
    const Triangle& currTri = *iter;
    // calc bb of triangle, to determine region of cells that can intersect
    float currentBounds[] = { UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH };
    for ( unsigned int i = 0; i < 3; ++i ) {
      const Vector3D& v = currTri.getPoint ( i );
      for ( unsigned char component = 0; component < 3 ; ++component ) {
        component_min = 2 * component;
        if ( v.value[component] < currentBounds[component_min] )
          currentBounds[component_min] = v.value[component];
        if ( v.value[component] > currentBounds[component_min+1] )
          currentBounds[component_min+1] = v.value[component];
      }
    }
    // trim boundings to gridcells
    unsigned int intbounds[6];

    intbounds[0] = ( int ) floorf ( ( currentBounds[0] - bounds[0] ) / cellsize ) ;
    intbounds[1] = ( int ) floorf ( ( currentBounds[1] - bounds[0] ) / cellsize ) + 1;
    intbounds[2] = ( int ) floorf ( ( currentBounds[2] - bounds[2] ) / cellsize ) ;
    intbounds[3] = ( int ) floorf ( ( currentBounds[3] - bounds[2] ) / cellsize ) + 1 ;
    intbounds[4] = ( int ) floorf ( ( currentBounds[4] - bounds[4] ) / cellsize ) ;
    intbounds[5] = ( int ) floorf ( ( currentBounds[5] - bounds[4] ) / cellsize ) + 1 ;

    for ( unsigned int xpos = intbounds[0]; xpos < intbounds[1] ; ++xpos )
      for ( unsigned int ypos = intbounds[2]; ypos < intbounds[3] ; ++ypos )
        for ( unsigned int zpos = intbounds[4]; zpos < intbounds[5] ; ++zpos ) {
          if ( TriAABBIntersect::intersects ( Vector3D ( bounds[0] + xpos * cellsize + cellsizeHalf, bounds[2] + ypos * cellsize + cellsizeHalf, bounds[4] + zpos * cellsize + cellsizeHalf ), boxhalfSize, currTri ) ) {

            const unsigned int cellIdx =   zpos * slabSize // slabs in z-direction
                                           + ypos * cellCount[0] // + y-count rows
                                           + xpos;
            if ( cellIdx > gridsize ) {
              std::cerr << "IndexOutOfBounds: Tried to access non existent RegularGrid cell\n";
            }
            if ( grid[cellIdx] == 0 ) {
              grid[cellIdx] = new std::vector<int>;
            }

            grid[cellIdx]->push_back ( trianglesIdx );
          }
        }

    ++trianglesIdx;
  }
}
