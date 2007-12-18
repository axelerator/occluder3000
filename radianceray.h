//
// C++ Interface: radianceray
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RADIANCERAY_H
#define RADIANCERAY_H
#include "vector3d.h"
// #include "triangle.h"
#include "accelerationstruct.h"
#include "intersection.h"
#include "rgbvalue.h"
#include "scene.h"
#include "ray.h"
/**
A ray that gathers radiance when cast into a scene.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RadianceRay : public Ray{
  public:
    RadianceRay( Scene& scene):scene(scene){}

    ~RadianceRay();
    const bool didHitSomething() const { return closestIntersection.triangle != 0 ; }
    Intersection& getClosestIntersection()  { return closestIntersection; }
    void setClosestIntersection(const Intersection& newInters) { closestIntersection = newInters; }

    void shade(RGBvalue& result) {
      if (closestIntersection.triangle != 0 ) {
        const Triangle &hitTriangle = * ( closestIntersection.triangle );
        Vector3D n ( hitTriangle.getNormalAt ( closestIntersection ) );
        const PhongMaterial& mat = hitTriangle.getMaterial();
        const std::vector<Light> lights = scene.getLights();
        std::vector<Light>::const_iterator it;
        IntersectionResult doesntMatter;
        float tmax;
        for ( it = lights.begin(); it!=lights.end(); ++it ) {
          const Light& light = *it;
          Vector3D l ( light.getPosition() -  closestIntersection.intersectionPoint );
          tmax = l.normalizeRL();
          
          Ray intersectToLigth ( closestIntersection.intersectionPoint, l, tmax );
          float dif = n * l;
          if ( dif > 0.0 ) {
            
            if ( !scene.getGeometry().isBlocked(intersectToLigth, &hitTriangle) ) { //shadowtest
              result.add ( dif * mat.diffuse[0] * light.getColor().getRGB() [0],
                           dif * mat.diffuse[1] * light.getColor().getRGB() [1],
                           dif * mat.diffuse[2] * light.getColor().getRGB() [2] );
            }
          }
      
        }
      }    
    }    

  private:


    // hit info
    Intersection closestIntersection;
    const Scene &scene;
};

#endif
