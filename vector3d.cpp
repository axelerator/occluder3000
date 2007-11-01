//
// C++ Implementation: vector3d
//
// Description: 
//
//
// Author: Axel Tetzlaff / Timo B. Hübel <axel.tetzlaff@gmx.de / t.h@gmx.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cmath>

#include "vector3d.h"

fliess (*Fliess::abs)(fliess) = fabsf;
fliess (*Fliess::ceil)(fliess) = ceilf;
fliess (*Fliess::floor)(fliess) = floorf;
fliess (*Fliess::max)(fliess, fliess) = fmaxf;
fliess (*Fliess::min)(fliess, fliess) = fminf;

/**
 * Create a zero vector.
 */
Vector3D::Vector3D() {
  value[0] = 0.0;
  value[1] = 0.0;
  value[2] = 0.0;
}

/**
 * Create a copy of another vector. This is the copy-constructor.
 *
 * @param v The other vector.
 */
Vector3D::Vector3D(const Vector3D& v) {
  value[0] = v.value[0];
  value[1] = v.value[1];
  value[2] = v.value[2];
}

/**
 * Create vector from an arry.
 *
 * @param v A vector as array.
 */
Vector3D::Vector3D(const fliess *v) {
  value[0] = v[0];
  value[1] = v[1];
  value[2] = v[2];
}

/**
 * Create a vector from three values.
 *
 * @param fx value[0]-value.
 * @param fy value[1]-value.
 * @param fz value[2]-value.
 */
Vector3D::Vector3D(fliess fx, fliess fy, fliess fz) {
  value[0] = fx;
  value[1] = fy;
  value[2] = fz;
}

/**
 * Create a vector with the same value for value[0], value[1] and value[2].
 *
 * @param fn The value.
 */
Vector3D::Vector3D(fliess fn) {
  value[0] = value[1] = value[2] = fn;
}

Vector3D::~Vector3D() {
}

Vector3D& Vector3D::operator += (const Vector3D& v) {
  value[0] += v.value[0];
  value[1] += v.value[1];
  value[2] += v.value[2];
  return (*this);
}

Vector3D& Vector3D::operator -= (const Vector3D& v) {
  value[0] -= v.value[0];
  value[1] -= v.value[1];
  value[2] -= v.value[2];
  return (*this);
}

Vector3D& Vector3D::operator *= (fliess f) {
  value[0] *= f;
  value[1] *= f;
  value[2] *= f;
  return (*this);
}

Vector3D& Vector3D::operator /= (fliess f) {
  fliess s = 1.0f/f;
  value[0] *= s;
  value[1] *= s;
  value[2] *= s;
  return (*this);
}

Vector3D& Vector3D::operator = (const Vector3D& v) {
  value[0] = v.value[0];
  value[1] = v.value[1];
  value[2] = v.value[2];
  return (*this);
}

Vector3D Vector3D::operator + () const {
  return Vector3D(*this);
}

Vector3D Vector3D::operator - () const {
  return Vector3D(-value[0], -value[1], -value[2]);
}

Vector3D Vector3D::operator + (const Vector3D& v) const {
  return Vector3D(value[0]+v.value[0], value[1]+v.value[1], value[2]+v.value[2]);
}

Vector3D Vector3D::operator - (const Vector3D& v) const {
  return Vector3D(value[0]-v.value[0], value[1]-v.value[1], value[2]-v.value[2]);
}

/**
 * Calculate the cross product of the vector and another vector.
 *
 * @param v The other vector.
 * @return The cross product of the two vectors.
 */
Vector3D Vector3D::operator % (const Vector3D& v) const {
  return Vector3D(value[1]*v.value[2] - value[2]*v.value[1], value[2]*v.value[0] - value[0]*v.value[2], value[0]*v.value[1] - value[1]*v.value[0]);
}

bool Vector3D::operator > (const Vector3D& v) const {
  return sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]) > sqrt(v.value[0]*v.value[0] + v.value[1]*v.value[1] + v.value[2]*v.value[2]);
}

bool Vector3D::operator < (const Vector3D& v) const {
  return sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]) < sqrt(v.value[0]*v.value[0] + v.value[1]*v.value[1] + v.value[2]*v.value[2]);
}

Vector3D Vector3D::operator + (fliess f) const {
  return Vector3D(value[0]+f, value[1]+f, value[2]+f);
}

Vector3D Vector3D::operator - (fliess f) const {
  return Vector3D(value[0]-f, value[1]-f, value[2]-f);
}

Vector3D Vector3D::operator * (fliess f) const {
  return Vector3D(value[0]*f, value[1]*f, value[2]*f);
}

Vector3D Vector3D::operator / (fliess f) const {
  fliess s = 1.0f/f;
  return Vector3D(value[0]*s, value[1]*s, value[2]*s);
}

bool Vector3D::operator == (const Vector3D& v) const {
  if ((value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2])) return true;
  return false;
}

bool Vector3D::operator != (const Vector3D& v) const {
  if ((value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2])) return false;
  return true;
}

/**
 * Allowing indexed access to the three members. This is here
 * just for compatability!!!
 */
fliess& Vector3D::operator [] (unsigned int i) {
  switch (i) {
   case 0: return value[0]; break;
   case 1: return value[1]; break;
   case 2: return value[2];
 }

  // We should throw some exception here...
  return value[0];
}

fliess Vector3D::operator [] (unsigned int i) const {
  switch (i) {
   case 0: return value[0]; break;
   case 1: return value[1]; break;
   case 2: return value[2];
 }

  // We should throw some exception here...
  return 0.0;
}

/**
 * Calculates the dot product of the vector and another vector.
 *
 * @param v The other vector.
 * @return The dot product of the two vectors.
 */
fliess Vector3D::operator * (const Vector3D& v) const { 
  return value[0]*v.value[0] + value[1]*v.value[1] + value[2]*v.value[2];
}

/**
 * Calculates the length of the vector.
 *
 * @return Length of the vector.
 */
fliess Vector3D::length() const {
  return static_cast<fliess>(sqrt(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]));
}

/**
 * Calculate the squared length of the vector.
 *
 * @return Squared length of the vector.
 */
fliess Vector3D::lengthSquare() const {
  return static_cast<fliess>(value[0]*value[0] + value[1]*value[1] + value[2]*value[2]);
}

/**
 * Return a normalzed version of this vector.
 *
 * @return The normalized vector.
 */
Vector3D Vector3D::normal() const {
  fliess s = 1.0f / length();
  return Vector3D(value[0]*s, value[1]*s, value[2]*s);
}

const Vector3D operator+(fliess lhs, const Vector3D& rhs) {
  return Vector3D(lhs + rhs.value[0], lhs + rhs.value[1], lhs + rhs.value[2]);
}

const Vector3D operator*(fliess lhs, const Vector3D& rhs) {
  return Vector3D(lhs * rhs.value[0], lhs * rhs.value[1], lhs * rhs.value[2]);
}

std::ostream& operator << (std::ostream& os, const Vector3D& v) {
  return os << "value[0]: " << v.value[0] << " value[1]: " << v.value[1] << " value[2]: " << v.value[2] << std::endl;
}



/**
 * Normalize the vector, that is, make it's length one.
 *
 * @return The normalized vector.
 */
Vector3D& Vector3D::normalize()
{
 fliess s = 1.0f / length();
 this->operator *=( s );
 return (*this);
}


/**
 * @return the angle between this and the provided vector
 */
fliess Vector3D::angleTo(Vector3D& rhs)
{
    return acos(operator *(rhs)/(length()*rhs.length()));
}

