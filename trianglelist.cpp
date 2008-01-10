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



const RGBvalue Trianglelist::trace ( RadianceRay& rr, unsigned int depth ) {
  for ( std::vector<Triangle>::iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    (*it).intersect( rr );
  }
  RGBvalue result ( 0.0, 0.0, 0.0 );
  rr.shade( result, depth );

  return result;
}

bool Trianglelist::trace ( RayPacket& rp, unsigned int depth ) {
  for ( std::vector<Triangle>::iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    (*it).intersect( rp );
  }

  rp.shade( depth );

  return true;
}

bool Trianglelist::isBlocked(Ray& r) {
  for ( std::vector<Triangle>::iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    const Triangle& tri = (*it);
    if ( tri.intersect( r ) )
      return true;
  }
  return false;
}

void Trianglelist::construct() {}
