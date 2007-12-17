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
#include "intersection.h"


/**
A ray that gathers radiance when cast into a scene.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class RadianceRay {
  public:
    RadianceRay(const Vector3D& s, const Vector3D& d, float max, float min):start(s), direction(d), invDirection(1.0/d.value[0], 1.0/d.value[1], 1.0/d.value[2]), tMax(max), tMin(min) {};

    ~RadianceRay();
    void setDirection(const Vector3D& direction) { this->direction = direction;  invDirection.value[0] = 1.0/direction.value[0]; invDirection.value[1] =  1.0/direction.value[1]; invDirection.value[2] = 1.0/direction.value[2]; }
    void setStart(const Vector3D& start) {  this->start = start;    }
    const Vector3D& getStart() const { return start; }
    const Vector3D& getDirection() const { return direction; }
    const Vector3D& getInvDirection() const { return invDirection; }
    const bool didHitSomething() const { return closestIntersection.triangle != 0 ; }
    Intersection& getClosestIntersection()  { return closestIntersection; }
    void setClosestIntersection(const Intersection& newInters) { closestIntersection = newInters; }
    void setMax(float max) { tMax = max; }
    float getMax() { return tMax; }    
    void setMin(float min) { tMin = min; }
    float getMin() { return tMin; }    
    

    void shade(Intersection& inters, RGBvalue& result) {
      if (closestIntersection.triangle != 0 ) {
        const Triangle &hitTriangle = * ( closestIntersection.triangle );
        Vector3D n ( hitTriangle.getNormalAt ( closestIntersection ) );
        const PhongMaterial& mat = hitTriangle.getMaterial();
        const std::vector<Light> lights = scene->getLights();
        std::vector<Light>::const_iterator it;
        IntersectionResult doesntMatter;
        float tmax;
        for ( it = lights.begin(); it!=lights.end(); ++it ) {
          const Light& light = *it;
          Vector3D l ( light.getPosition() -  closestIntersection.intersectionPoint );
          tmax = l.length();
          l.normalize();
          Ray intersectToLigth ( closestIntersection.intersectionPoint, l );
          float dif = n * l;
          if ( dif > 0.0 ) {
            if ( 1 ) { //shadowtest
              result.add ( dif * mat.diffuse[0] * light.getColor().getRGB() [0],
                           dif * mat.diffuse[1] * light.getColor().getRGB() [1],
                           dif * mat.diffuse[2] * light.getColor().getRGB() [2] );
            }
          }
      
        }
      }    
    }    
    void setScene(const Scene& s) { scene = &s; }
  private:
    Vector3D start;
    Vector3D direction;
    Vector3D invDirection;
    float tMax;
    float tMin;
    // hit info
    Intersection closestIntersection;
    Scene *scene;
};

#endif
