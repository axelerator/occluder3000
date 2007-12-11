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

Trianglelist::Trianglelist ( const Scene& scene ) :
    AccelerationStruct ( scene ) {}

Trianglelist::~Trianglelist() {}



const RGBvalue Trianglelist::trace ( Ray& r, unsigned int depth ) {
  IntersectionResult ir;

  float zvalue = 10000.0;
  float currentZ;
  Triangle* closestTriangle = 0;
  Vector3D poi;
  Vector3D finalPoi;
  for ( std::vector<Triangle>::iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
    if ( ( *it ).intersect ( r,ir ) ) {
      poi = ir.calcPOI();
      currentZ = ( poi - r.getStart() ).lengthSquare();
      if ( currentZ > 0 && currentZ < zvalue ) {
        zvalue = currentZ;
        closestTriangle = & ( *it );
        finalPoi = poi;
      }
    }
  }
  if ( closestTriangle != 0 )  {

    Vector3D n ( closestTriangle->getNormalAt ( ir ) );
    RGBvalue lightVessel ( 0.0, 0.0, 0.0 );
    const PhongMaterial& mat = closestTriangle->getMaterial();
    const std::vector<Light> lights = scene.getLights();
    std::vector<Light>::const_iterator it;
    IntersectionResult iir;
    unsigned int tc = triangles.size();
    for ( it = lights.begin(); it!=lights.end(); ++it ) {
      const Light& light = *it;
//            Vector3D l ( finalPoi - light.getPosition() );
      Vector3D l ( light.getPosition() - finalPoi );
      l.normalize();
      Ray shadowRay ( ir.getPOI(), l );
      float dif = n * l;
      //dif = 1.0;
      if ( dif > 0.0 ) {

        bool hitt = false;
        for ( unsigned int foo = 0; foo < tc; ++foo ) {
          const Triangle &st = triangles[foo];
          if ( &st != closestTriangle ) {
            if ( st.intersect ( shadowRay, iir ) ) {
              hitt = true;
              break;
            }
          }
        }
        if ( !hitt ) {
          lightVessel.add ( dif * mat.diffuse[0] * light.getColor().getRGB() [0],
                            dif * mat.diffuse[1] * light.getColor().getRGB() [1],
                            dif * mat.diffuse[2] * light.getColor().getRGB() [2] );
        }
      }
    }
    return lightVessel;
  } else
    return  RGBvalue ( 0, 0, 0 );
}


void Trianglelist::construct() {}
