//
// C++ Interface: triangle
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vector3d.h"
#include "phongmaterial.h"
#include "intersectionresult.h"
class Intersection;
class RadianceRay;
class Ray;
class RayPacket;
class Ray4;
class AccelerationStruct;
/* Evil cast to store 4 Triindices in 128Bitfloat */
typedef union {
  unsigned int i;
  float f;
} Intfloat ;

/**
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Triangle {
  public:
    Triangle(const unsigned int v1,const unsigned int v2,const unsigned int v3, const PhongMaterial& mat, const AccelerationStruct &ast);
    Triangle(const unsigned int v1,const unsigned int v2,const unsigned int v3, const Vector3D& vn1, const Vector3D& vn2, const Vector3D& vn3, const PhongMaterial& mat,  const AccelerationStruct &ast);    
    Triangle& operator= (const Triangle& cpy) { memcpy(this, &cpy, sizeof(Triangle)); return *this; }
    bool intersect(const Ray& r) const;
    bool intersect(RadianceRay& r) const;
    void intersect(RayPacket& rp, unsigned int idx) const;
    bool intersect(Ray4& rp, unsigned int idx) const;
    const PhongMaterial& getMaterial() const { return mat; }
    Vector3D getNormalAt(const IntersectionResult& ir) const;
    Vector3D getNormalAt(const Intersection& ir) const;
    Vector3D getNormal() const { return normal;}
    SSEVec3D getNormalsAt() const { return SSEVec3D(normal);}
    const Vector3D& getPoint(unsigned int i) const;
    const Vector3D& getCenter() const { return center; }
    const Vector3D& getU() const { return u; }
    bool isSpecular() const { return mat.isSpecular();}
    bool isDiffuse() const { return mat.isDiffuse();}

    ~Triangle();
  private:

    const unsigned int p0, p1, p2;

    Vector3D u;
    Vector3D v;
    Vector3D normal;
    Vector3D nu;
    Vector3D nv;
    Vector3D center;
    const PhongMaterial& mat;
    const AccelerationStruct &as;

};

#endif
