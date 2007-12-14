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
#include "light.h"
#include "debug.h"
#include "radianceray.h"

Trianglelist::Trianglelist ( const Scene& scene ) :
    AccelerationStruct ( scene ) {}

Trianglelist::~Trianglelist() {}



const RGBvalue Trianglelist::trace ( Ray& r, unsigned int depth ) {
  RadianceRay rr ( r.getStart(), r.getDirection());
  for ( std::vector<Triangle>::iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    (*it).intersect( rr );
  }
  RGBvalue result ( 0.0, 0.0, 0.0 );
  if ( rr.didHitSomething() ) {
    const Intersection &i = rr.getClosestIntersection();
    const Triangle &hitTriangle = * ( i.triangle );
    Vector3D n ( hitTriangle.getNormalAt ( i ) );
    const PhongMaterial& mat = hitTriangle.getMaterial();
    const std::vector<Light> lights = scene.getLights();
    std::vector<Light>::const_iterator it;
    IntersectionResult doesntMatter;
    for ( it = lights.begin(); it!=lights.end(); ++it ) {
      const Light& light = *it;
      Vector3D l ( light.getPosition() -  i.intersectionPoint );
      l.normalize();
      Ray intersectToLigth ( i.intersectionPoint, l );
      float dif = n * l;
      if ( dif > 0.0 ) {
        result.add ( dif * mat.diffuse[0] * light.getColor().getRGB() [0],
                     dif * mat.diffuse[1] * light.getColor().getRGB() [1],
                     dif * mat.diffuse[2] * light.getColor().getRGB() [2] );
      }
    }
  }
  return result;
}


void Trianglelist::construct() {}
