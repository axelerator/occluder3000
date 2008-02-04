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
#include <assert.h>


#define EPSILON 0.0001f
//#define UNENDLICH INFINITY
#define UNENDLICH 1000.0

#define INLINE

#ifdef INLINE

class Vector3D {
public:
   inline ~Vector3D() {}

   /**
    Creates a vector from a string with three space seperated values.
    i.e.: " 0.12312 -1.32 .05 "
    **/
   Vector3D (const std::string& valuestr) ;

   inline  Vector3D() {
        value[0] = 0.0;
        value[1] = 0.0;
        value[2] = 0.0;
    }

    /**
     * Create a copy of another vector. This is the copy-constructor.
     *
     * @param v The other vector.
     */
//     inline Vector3D(const Vector3D& v) {
//         value[0] = v.value[0];
//         value[1] = v.value[1];
//         value[2] = v.value[2];
//     }

    /**
     * Create vector from an arry.
     *
     * @param v A vector as array.
     */
    inline Vector3D(const float *v) {
//         value[0] = v[0];
//         value[1] = v[1];
//         value[2] = v[2];
      memcpy(value, v, 3 * sizeof(float));
    }

    /**
     * Create a vector from three values.
     *
     * @param fx value[0]-value.
     * @param fy value[1]-value.
     * @param fz value[2]-value.
     */
    inline Vector3D(float fx, float fy, float fz) {
        value[0] = fx;
        value[1] = fy;
        value[2] = fz;
    }

    /**
     * Create a vector with the same value for value[0], value[1] and value[2].
     *
     * @param fn The value.
     */
    inline Vector3D(float fn) {
        value[0] = value[1] = value[2] = fn;
    }


    inline Vector3D& operator += (const Vector3D& v) {
        value[0] += v.value[0];
        value[1] += v.value[1];
        value[2] += v.value[2];
        return (*this);
    }

    inline Vector3D& operator -= (const Vector3D& v) {
        value[0] -= v.value[0];
        value[1] -= v.value[1];
        value[2] -= v.value[2];
        return (*this);
    }

    inline Vector3D& operator *= (float f) {
        value[0] *= f;
        value[1] *= f;
        value[2] *= f;
        return (*this);
    }

    inline Vector3D& operator /= (float f) {
        float s = 1.0f/f;
        value[0] *= s;
        value[1] *= s;
        value[2] *= s;
        return (*this);
    }

    inline Vector3D& operator = (const Vector3D& v) {
        memcpy(this, v.value, sizeof(Vector3D));
        return (*this);
    }

    inline Vector3D operator + () const {
        return Vector3D(*this);
    }

    inline Vector3D operator - () const {
        return Vector3D(-value[0], -value[1], -value[2]);
    }

    inline Vector3D operator + (const Vector3D& v) const {
        return Vector3D(value[0]+v.value[0], value[1]+v.value[1], value[2]+v.value[2]);
    }

    inline Vector3D operator - (const Vector3D& v) const {
        return Vector3D(value[0]-v.value[0], value[1]-v.value[1], value[2]-v.value[2]);
    }

    /**
     * Calculate the cross product of the vector and another vector.
     *
     * @param v The other vector.
     * @return The cross product of the two vectors.
     */
    inline Vector3D operator % (const Vector3D& v) const {
        return Vector3D(value[1]*v.value[2] - value[2]*v.value[1], value[2]*v.value[0] - value[0]*v.value[2], value[0]*v.value[1] - value[1]*v.value[0]);
    }

    inline bool operator > (const Vector3D& v) const {
        return sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]) > sqrt(v.value[0]*v.value[0] + v.value[1]*v.value[1] + v.value[2]*v.value[2]);
    }

    inline bool operator < (const Vector3D& v) const {
        return sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]) < sqrt(v.value[0]*v.value[0] + v.value[1]*v.value[1] + v.value[2]*v.value[2]);
    }

    inline Vector3D operator + (float f) const {
        return Vector3D(value[0]+f, value[1]+f, value[2]+f);
    }

    inline Vector3D operator - (float f) const {
        return Vector3D(value[0]-f, value[1]-f, value[2]-f);
    }

    inline Vector3D operator * (float f) const {
        return Vector3D(value[0]*f, value[1]*f, value[2]*f);
    }

    inline Vector3D operator / (float f) const {
        float s = 1.0f/f;
        return Vector3D(value[0]*s, value[1]*s, value[2]*s);
    }

    inline bool operator == (const Vector3D& v) const {
        if ((value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2]))
            return true;
        return false;
    }

    inline bool operator != (const Vector3D& v) const {
        if ((value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2]))
            return false;
        return true;
    }

    /**
     * Calculates the dot product of the vector and another vector.
     *
     * @param v The other vector.
     * @return The dot product of the two vectors.
     */
    inline float operator * (const Vector3D& v) const {
        return value[0]*v.value[0] + value[1]*v.value[1] + value[2]*v.value[2];
    }

    /**
     * Calculates the length of the vector.
     *
     * @return Length of the vector.
     */
    inline float length() const {
        return static_cast<float>(sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]));
    }

    /**
     * Calculate the squared length of the vector.
     *
     * @return Squared length of the vector.
     */
    inline float lengthSquare() const {
        return static_cast<float>(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]);
    }

    /**
     * Return a normalzed version of this vector.
     *
     * @return The normalized vector.
     */
    inline Vector3D normal() const {
        float s = 1.0f / length();
        return Vector3D(value[0]*s, value[1]*s, value[2]*s);
    }

   /**
     * Normalize the vector, that is, make it's length one.
     *
     * @return The normalized vector.
     */
    inline Vector3D& normalize() {
        float s = 1.0f / length();
        this->operator *=( s );
        return (*this);
    }

   /**
     * Normalize the vector, return legnth before
     *
     * @return The normalized vector.
     */
    inline float normalizeRL() {
        const float l = length();
        const float s = 1.0f / l;
        this->operator *=( s );
        return l;
    }

    /**
     * @return the angle between this and the provided vector
     */
    inline float angleTo(Vector3D& rhs) {
        return acos(operator *(rhs)/(length()*rhs.length()));
    }

    static Vector3D getRandomSphereVec() {
      assert(false);
      float angle1 = (rand() / (RAND_MAX + 1.0f))* 2.0f * M_PI;
      float angle2 = /*(rand() / (RAND_MAX + 1.0))**/ 2.0f * M_PI;
/*      return Vector3D(  cos(angle1)*sin(angle2),
                        cos(angle2),
                        sin(angle1)*sin(angle2));*/
        return Vector3D( cos(angle1) * sin(angle2),  cos(angle2), sin(angle1)) * sin(angle2);
    }

    /**
      @return the reflection of this at the geiven normal
    **/
    inline Vector3D reflect(const Vector3D& normal) const {
       const float vnn = ( *this * normal ) * 2.0f;
       return ( *this - ( normal * vnn ));
    }
    float value[3];
};
#else
/**
  @author Axel Tetzlaff / Timo B. Hübel <axel.tetzlaff@gmx.de / t.h@gmx.com>
*/
class Vector3D {
  public:
    Vector3D();
    Vector3D(const Vector3D& v);
    Vector3D(const float *v);
    Vector3D(float fx, float fy, float fz);
    Vector3D(float fn);

    virtual ~Vector3D();

    Vector3D& operator+=(const Vector3D& v);
    Vector3D& operator-=(const Vector3D& v);
    Vector3D& operator*=(float f);
    Vector3D& operator/=(float f);
    Vector3D& operator=(const Vector3D& v);
    Vector3D& normalize();

    Vector3D operator+() const;
    Vector3D operator-() const;

    Vector3D operator+(const Vector3D& v) const;
    Vector3D operator-(const Vector3D& v) const;

    Vector3D operator%(const Vector3D& v) const;

    Vector3D operator+(float f) const;
    Vector3D operator-(float f) const;
    Vector3D operator*(float f) const;
    Vector3D operator/(float f) const;

    float operator*(const Vector3D& v) const;

    bool operator>(const Vector3D& v) const;
    bool operator<(const Vector3D& v) const;

    bool operator==(const Vector3D& v) const;
    bool operator!=(const Vector3D& v) const;

    float& operator[](unsigned int i);
    float operator[](unsigned int i) const;

    float length() const;
    float lengthSquare() const;

    Vector3D normal() const;
    float angleTo(Vector3D& rhs);

    float value[3];

  private:
    static const Vector3D* undefined_ ;

};
#endif

const Vector3D operator+(float lhs, const Vector3D& rhs);
const Vector3D operator*(float lhs, const Vector3D& rhs);

std::ostream& operator<<(std::ostream& os, const Vector3D& v);

#endif
