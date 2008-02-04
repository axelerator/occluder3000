//
// C++ Implementation: trianglelist
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "trianglelist.h"
#include "debug.h"
#include "radianceray.h"
#include "raypacket.h"

Trianglelist::Trianglelist ( const Scene& scene ) :
    AccelerationStruct ( scene ) {}

Trianglelist::~Trianglelist() {}



const RGBvalue Trianglelist::trace ( RadianceRay& rr, unsigned int depth ) const {
  for ( std::vector<Triangle>::const_iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    (*it).intersect( rr );
  }
  RGBvalue result ( 0.0, 0.0, 0.0 );
  rr.shade( result, depth );

  return result;
}

bool Trianglelist::trace ( RayPacket& rp, unsigned int depth ) const {
  unsigned int i = 0;
  for ( std::vector<Triangle>::const_iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    (*it).intersect( rp, i++ );
  }
  return true;
}

const Intersection& Trianglelist::getClosestIntersection(RadianceRay& r) const {
  for ( std::vector<Triangle>::const_iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    (*it).intersect( r );
  }
  return r.getClosestIntersection();
}

bool Trianglelist::isBlocked(Ray& r) const {
  for ( std::vector<Triangle>::const_iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    const Triangle& tri = (*it);
    if ( tri.intersect( r ) )
      return true;
  }
  return false;
}

void Trianglelist::findAllIntersections(std::vector<Intersection> results, RadianceRay& r) {
  for ( std::vector<Triangle>::const_iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    if ( (*it).intersect( r ) )
      results.push_back( r.getClosestIntersection() );
  }
}

void Trianglelist::construct() {}
