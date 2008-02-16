//
// C++ Interface: vec3
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <string.h>
#include <string>
#include <ostream>

#define UNENDLICH INFINITY
#define EPSILON 0.00000000001f

namespace Occluder {
/**
Basic threedimensional vector.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Vec3{
public:
 friend const Vec3 Occluder::operator+(float lhs, const Vec3& rhs);
 friend const Vec3 Occluder::operator*(float lhs, const Vec3& rhs);
 friend std::ostream& operator << (std::ostream& os, const Vec3& v);
    Vec3();
    ~Vec3();

    const float* getFloatArray() const ;

    Vec3(const float *v);
    Vec3(float fx, float fy, float fz);
    Vec3(float fn);
    Vec3 (const std::string& valuestr);

    Vec3& operator+=(const Vec3& v);
    Vec3& operator-=(const Vec3& v);
    Vec3& operator*=(float f);
    Vec3& operator/=(float f);
    Vec3& operator=(const Vec3& v);

    /**
      Scales the vector componentwise with another vector.
      Example: Vec3 a(1.0, 2.0, 3.0); a^=Vec3(0.5, 2.0, 1.0) // a => (0.5, 4.0, 3.)
      @param v the vector this vector is to be scaled by
      @return the scaled vector
     **/
    Vec3& operator^=(const Vec3& v);

    /**
      Componentwise multiplication. @see Vec3#operator^=
      @param op multiplicant
      @return Vector where each component contains the product of the 
              accordant componets of the operands.
     **/
    Vec3 operator^(const Vec3& op) const;

    /**
      Normalizes this vector.
      @return the normalized instance
     **/
    Vec3& normalize();
    
    /**
      Normalizes the vector and returns the previous length.
      @return length before normalizing;
    **/
    float normalizeRL();

    Vec3 operator+() const;
    Vec3 operator-() const;

    Vec3 operator+(const Vec3& v) const;
    Vec3 operator-(const Vec3& v) const;

    Vec3 operator%(const Vec3& v) const;

    Vec3 operator+(float f) const;
    Vec3 operator-(float f) const;
    Vec3 operator*(float f) const;
    Vec3 operator/(float f) const;

    float operator*(const Vec3& v) const;

    float& operator[](unsigned int i);
    float operator[](unsigned int i) const;

    float length() const;
    float lengthSquare() const;

    /**
      @return the vector resized to length of 1.
     **/
    Vec3 normal() const;
    
    /**
      @param rhs the operand
      @return the angle between this vector and the operand
     **/
    float angleTo(Vec3& rhs);
  
    /** 
       calculates the reflection of a vector at a surface 
       @param normal the normal of the surface this vectore gets reflected at
       @return the ideally, on the surface of plane with normal, reflected vector
     **/
    Vec3 reflect(const Vec3& normal) const;

    /**
      Calculates the refracted direction of an transmitted ray.
      @param n normal of the surface
      @param nFrom refractive index of the material the ray is comming from
      @param nTo refractive index of the material the ray enters
     **/
    Vec3 refract(const Vec3& n, float nFrom, float nTo ) const;

    static Vec3 getRandomSphereVec();


private:
  float c[3];

};
    const Vec3 operator+(float lhs, const Vec3& rhs);
    
    const Vec3 operator*(float lhs, const Vec3& rhs);

}

using namespace Occluder;
std::ostream& operator << (std::ostream& os, const Vec3& v);

// ------------------ implementation for inlined methods -----------------------

inline Vec3::Vec3() {}

    /**
     * Create vector from an arry.
     *
     * @param v A vector as array.
     */
    inline Vec3::Vec3(const float *v) {
      memcpy(c, v, 3 * sizeof(Vec3));
    }

    /**
     * Create a vector from three cs.
     *
     * @param fx c[0]-c.
     * @param fy c[1]-c.
     * @param fz c[2]-c.
     */
    inline Vec3::Vec3(float fx, float fy, float fz) {
        c[0] = fx;
        c[1] = fy;
        c[2] = fz;
    }

    /**
     * Create a vector with the same c for c[0], c[1] and c[2].
     *
     * @param fn The c.
     */
    inline Vec3::Vec3(float fn) {
        c[0] = c[1] = c[2] = fn;
    }

    inline const float* Vec3::getFloatArray() const {
      return c;
    }


    inline Vec3& Vec3::operator += (const Vec3& v) {
        c[0] += v.c[0];
        c[1] += v.c[1];
        c[2] += v.c[2];
        return (*this);
    }

    inline Vec3& Vec3::operator -= (const Vec3& v) {
        c[0] -= v.c[0];
        c[1] -= v.c[1];
        c[2] -= v.c[2];
        return (*this);
    }

    inline Vec3& Vec3::operator *= (float f) {
        c[0] *= f;
        c[1] *= f;
        c[2] *= f;
        return (*this);
    }

    inline Vec3& Vec3::operator /= (float f) {
        float s = 1.0f/f;
        c[0] *= s;
        c[1] *= s;
        c[2] *= s;
        return (*this);
    }

    inline Vec3& Vec3::operator = (const Vec3& v) {
        memcpy(this, v.c, sizeof(Vec3));
        return (*this);
    }

    inline Vec3& Vec3::operator^=(const Vec3& v) {
      c[0] *= v.c[0];
      c[1] *= v.c[1];
      c[2] *= v.c[2];
      return *this;
    }

    inline Vec3 Vec3::operator^(const Vec3& op) const {
      return Vec3(c[0] * op.c[0], c[1] * op.c[1], c[2] * op.c[2] );
    }

    inline Vec3 Vec3::operator + () const {
        return Vec3(*this);
    }

    inline Vec3 Vec3::operator - () const {
        return Vec3(-c[0], -c[1], -c[2]);
    }

    inline Vec3 Vec3::operator + (const Vec3& v) const {
        return Vec3(c[0]+v.c[0], c[1]+v.c[1], c[2]+v.c[2]);
    }

    inline Vec3 Vec3::operator - (const Vec3& v) const {
        return Vec3(c[0]-v.c[0], c[1]-v.c[1], c[2]-v.c[2]);
    }

    /**
     * Calculate the cross product of the vector and another vector.
     *
     * @param v The other vector.
     * @return The cross product of the two vectors.
     */
    inline Vec3 Vec3::operator % (const Vec3& v) const {
        return Vec3(c[1]*v.c[2] - c[2]*v.c[1], c[2]*v.c[0] - c[0]*v.c[2], c[0]*v.c[1] - c[1]*v.c[0]);
    }

    inline Vec3 Vec3::operator + (float f) const {
        return Vec3(c[0]+f, c[1]+f, c[2]+f);
    }

    inline Vec3 Vec3::operator - (float f) const {
        return Vec3(c[0]-f, c[1]-f, c[2]-f);
    }

    inline Vec3 Vec3::operator * (float f) const {
        return Vec3(c[0]*f, c[1]*f, c[2]*f);
    }

    inline Vec3 Vec3::operator / (float f) const {
        const float s = 1.0f/f;
        return Vec3(c[0]*s, c[1]*s, c[2]*s);
    }



    inline float& Vec3::operator[](unsigned int i) {
      return c[i];
    }

    inline float Vec3::operator[](unsigned int i) const {
      return c[i];
    }



    /**
     * Calculates the dot product of the vector and another vector.
     *
     * @param v The other vector.
     * @return The dot product of the two vectors.
     */
    inline float Vec3::operator * (const Vec3& v) const {
        return c[0]*v.c[0] + c[1]*v.c[1] + c[2]*v.c[2];
    }

    /**
     * Calculates the length of the vector.
     *
     * @return Length of the vector.
     */
    inline float Vec3::length() const {
        return static_cast<float>(sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]));
    }

    /**
     * Calculate the squared length of the vector.
     *
     * @return Squared length of the vector.
     */
    inline float Vec3::lengthSquare() const {
        return static_cast<float>(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]);
    }

    /**
     * Return a normalzed version of this vector.
     *
     * @return The normalized vector.
     */
    inline Vec3 Vec3::normal() const {
        const float s = 1.0f / length();
        return Vec3(c[0]*s, c[1]*s, c[2]*s);
    }

   /**
     * Normalize the vector, that is, make it's length one.
     *
     * @return The normalized vector.
     */
    inline Vec3& Vec3::normalize() {
        float s = 1.0f / length();
        this->operator *=( s );
        return (*this);
    }

   /**
     * Normalize the vector, return legnth before
     *
     * @return The normalized vector.
     */
    inline float Vec3::normalizeRL() {
        const float l = length();
        const float s = 1.0f / l;
        this->operator *=( s );
        return l;
    }

    /**
     * @return the angle between this and the provided vector
     */
    inline float Vec3::angleTo(Vec3& rhs) {
        return acos(operator *(rhs)/(length()*rhs.length()));
    }

    /**
      @return the reflection of this at the geiven normal
    **/
    inline Vec3 Vec3::reflect(const Vec3& normal) const {
       const float vnn = ( *this * normal ) * 2.0f;
       return ( *this - ( normal * vnn ));
    }

    inline const Vec3 Occluder::operator+(float lhs, const Vec3& rhs) {
      return Vec3(lhs + rhs.c[0], lhs + rhs.c[1], lhs + rhs.c[2]);
    }
    
    inline const Vec3 Occluder::operator*(float lhs, const Vec3& rhs) {
      return Vec3(lhs * rhs.c[0], lhs * rhs.c[1], lhs * rhs.c[2]);
    }
#endif
