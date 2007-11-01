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
class Ray;
/**
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Triangle
{
  public:
    Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3);
    Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, const Vector3D& vn1, const Vector3D& vn2, const Vector3D& vn3);    
    bool intersect(const Ray& r, IntersectionResult& ir) const;
    const PhongMaterial& getMaterial() const { return mat; }
    Vector3D getNormalAt(const IntersectionResult& ir) const;
    const Vector3D& getPoint(unsigned int i) const;
    const Vector3D& getCenter() const { return center; }
    ~Triangle();
  private:
    Vector3D p[3];
    PhongMaterial mat;
    Vector3D u;
    Vector3D v;
    Vector3D nu;
    Vector3D nv;
    Vector3D n[3];
    Vector3D center;
};

#endif
