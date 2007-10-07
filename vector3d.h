//
// C++ Interface: vector3d
//
// Description:
//
//
// Author: Axel Tetzlaff / Timo B. Hübel <axel.tetzlaff@gmx.de / t.h@gmx.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <iostream>
#include <sstream>
#include <math.h>


typedef float fliess;
#define EPSILON 0.00000001
#define UNENDLICH INFINITY

class Fliess {
  public:
    static fliess (*abs)(fliess) ;
    static fliess (*ceil)(fliess);
    static fliess (*floor)(fliess);
};

/**
  @author Axel Tetzlaff / Timo B. Hübel <axel.tetzlaff@gmx.de / t.h@gmx.com>
*/
class Vector3D {
  public:
    Vector3D();
    Vector3D(const Vector3D& v);
    Vector3D(const fliess *v);
    Vector3D(fliess fx, fliess fy, fliess fz);
    Vector3D(fliess fn);

    virtual ~Vector3D();

    Vector3D& operator+=(const Vector3D& v);
    Vector3D& operator-=(const Vector3D& v);
    Vector3D& operator*=(fliess f);
    Vector3D& operator/=(fliess f);
    Vector3D& operator=(const Vector3D& v);
    Vector3D& normalize();

    Vector3D operator+() const;
    Vector3D operator-() const;

    Vector3D operator+(const Vector3D& v) const;
    Vector3D operator-(const Vector3D& v) const;

    Vector3D operator%(const Vector3D& v) const;

    Vector3D operator+(fliess f) const;
    Vector3D operator-(fliess f) const;
    Vector3D operator*(fliess f) const;
    Vector3D operator/(fliess f) const;

    fliess operator*(const Vector3D& v) const;

    bool operator>(const Vector3D& v) const;
    bool operator<(const Vector3D& v) const;

    bool operator==(const Vector3D& v) const;
    bool operator!=(const Vector3D& v) const;

    fliess& operator[](unsigned int i);
    fliess operator[](unsigned int i) const;

    fliess length() const;
    fliess lengthSquare() const;

    Vector3D normal() const;
    fliess angleTo(Vector3D& rhs);

    fliess value[3];

  private:
    static const Vector3D* undefined_ ;

};

const Vector3D operator+(fliess lhs, const Vector3D& rhs);
const Vector3D operator*(fliess lhs, const Vector3D& rhs);

std::ostream& operator<<(std::ostream& os, const Vector3D& v);

#endif
