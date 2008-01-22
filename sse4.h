//
// C++ Interface: sse4
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SSE4_H
#define SSE4_H
#include <string.h>
#include <xmmintrin.h>
#include "vector3d.h"
#include "rgbvalue.h"
/**
Wrapper for SSE operations

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class SSE4 {
  public:
    SSE4(){}
//     SSE4(float x0, float x1, float x2, float x3 );
    SSE4(const SSE4& o) { memcpy(this->v.f, o.v.f, sizeof(o));}
    SSE4(const __m128 d) {v.sse = d;}
    SSE4(const float f) {v.sse = _mm_set_ps1(f);}
//     SSE4& operator=(const __m128& m) {v.sse = m;return *this;}
    SSE4 operator+(const SSE4& rhs) const;
    SSE4 operator*(const SSE4& rhs) const;
    ~SSE4(){}
  
  union {
    float f[4];
    __m128 sse;
  } v;

};

class SSEVec3D {
  public:
  SSEVec3D(){}
  SSEVec3D(const Vector3D& v){
   c[0] = _mm_set_ps1(v.value[0]);
   c[1] = _mm_set_ps1(v.value[1]);
   c[2] = _mm_set_ps1(v.value[2]);
  }
  SSEVec3D(const float* v){
   c[0] = _mm_set_ps1(v[0]);
   c[1] = _mm_set_ps1(v[1]);
   c[2] = _mm_set_ps1(v[2]);
  } 
 
  static void cross(const SSEVec3D& a, const SSEVec3D& b, SSEVec3D& dest);
  SSEVec3D operator%(const SSEVec3D& b) const;
  __m128 operator*(const SSEVec3D& b) const;
  SSEVec3D operator*(const SSE4& b) const;
  SSEVec3D operator^(const SSEVec3D& b) const;
  SSEVec3D operator-(const SSEVec3D& r) const;
  Vector3D get(unsigned int i) const;
  SSEVec3D& operator+=(const SSEVec3D& op);
  SSEVec3D& operator*=(const SSE4& op);
  SSEVec3D& operator-=(const SSEVec3D& op);
  SSEVec3D& operator=(const Vector3D& op);
  SSEVec3D& operator=(const __m128& op);
  __m128 normalizeRL();
  __m128 length() const;
  void setVec(unsigned int i, const Vector3D& v);
  void setVec(unsigned int i, const float *v);
  
  SSE4 c[3];
};

static const __m128 EPSILON4 = _mm_set_ps1(EPSILON);
static const __m128 EPSILON4INV = _mm_set_ps1(-EPSILON);
static const __m128 ONE = _mm_set_ps1(1.0f);



inline SSE4 SSE4::operator+(const SSE4& rhs) const {
  return _mm_add_ps(this->v.sse, rhs.v.sse);
}

inline SSE4 SSE4::operator*(const SSE4& rhs) const {
  return _mm_mul_ps(this->v.sse, rhs.v.sse);
}

inline void SSEVec3D::cross(const SSEVec3D& a, const SSEVec3D& b, SSEVec3D& dest) {
  dest.c[0].v.sse = _mm_sub_ps( _mm_mul_ps(a.c[1].v.sse, b.c[2].v.sse), _mm_mul_ps(a.c[2].v.sse, b.c[1].v.sse));
  dest.c[1].v.sse = _mm_sub_ps( _mm_mul_ps(a.c[2].v.sse, b.c[0].v.sse), _mm_mul_ps(a.c[0].v.sse, b.c[2].v.sse));
  dest.c[2].v.sse = _mm_sub_ps( _mm_mul_ps(a.c[0].v.sse, b.c[1].v.sse), _mm_mul_ps(a.c[1].v.sse, b.c[0].v.sse));
}

inline SSEVec3D SSEVec3D::operator%(const SSEVec3D& b) const {
  SSEVec3D result;
  result.c[0].v.sse = _mm_sub_ps( _mm_mul_ps(this->c[1].v.sse, b.c[2].v.sse), _mm_mul_ps(this->c[2].v.sse, b.c[1].v.sse));
  result.c[1].v.sse = _mm_sub_ps( _mm_mul_ps(this->c[2].v.sse, b.c[0].v.sse), _mm_mul_ps(this->c[0].v.sse, b.c[2].v.sse));
  result.c[2].v.sse = _mm_sub_ps( _mm_mul_ps(this->c[0].v.sse, b.c[1].v.sse), _mm_mul_ps(this->c[1].v.sse, b.c[0].v.sse));
  return result;
}

inline __m128 SSEVec3D::operator*(const SSEVec3D& b) const {
  return _mm_add_ps( _mm_add_ps(_mm_mul_ps(c[0].v.sse, b.c[0].v.sse) ,
                              _mm_mul_ps(c[1].v.sse, b.c[1].v.sse) ),
                              _mm_mul_ps(c[2].v.sse, b.c[2].v.sse) );
}

inline SSEVec3D SSEVec3D::operator*(const SSE4& b) const {
  SSEVec3D result;
  result.c[0].v.sse = _mm_mul_ps (this->c[0].v.sse, b.v.sse);
  result.c[1].v.sse = _mm_mul_ps (this->c[1].v.sse, b.v.sse);
  result.c[2].v.sse = _mm_mul_ps (this->c[2].v.sse, b.v.sse);
  return result;
}

/**
  Componentwise multiplication
**/
inline SSEVec3D SSEVec3D::operator^(const SSEVec3D& b) const {
  SSEVec3D result;
  result.c[0].v.sse = _mm_mul_ps(this->c[0].v.sse, b.c[0].v.sse);
  result.c[1].v.sse = _mm_mul_ps(this->c[1].v.sse, b.c[1].v.sse);
  result.c[2].v.sse = _mm_mul_ps(this->c[2].v.sse, b.c[2].v.sse);
  return result;
}

inline SSEVec3D SSEVec3D::operator-(const SSEVec3D& r) const {
  SSEVec3D result;
  result.c[0].v.sse = _mm_sub_ps (this->c[0].v.sse, r.c[0].v.sse);
  result.c[1].v.sse = _mm_sub_ps (this->c[1].v.sse, r.c[1].v.sse);
  result.c[2].v.sse = _mm_sub_ps (this->c[2].v.sse, r.c[2].v.sse);
  return result;
}

inline Vector3D SSEVec3D::get(unsigned int i) const {
  return Vector3D (c[0].v.f[i],c[1].v.f[i],c[2].v.f[i] );
}


inline SSEVec3D& SSEVec3D::operator+=(const SSEVec3D& op) {
  c[0].v.sse = _mm_add_ps (c[0].v.sse, op.c[0].v.sse);
  c[1].v.sse = _mm_add_ps (c[1].v.sse, op.c[1].v.sse);
  c[2].v.sse = _mm_add_ps (c[2].v.sse, op.c[2].v.sse);
  return *this;
}

inline SSEVec3D& SSEVec3D::operator*=(const SSE4& op) {
  c[0].v.sse = _mm_mul_ps (c[0].v.sse, op.v.sse);
  c[1].v.sse = _mm_mul_ps (c[1].v.sse, op.v.sse);
  c[2].v.sse = _mm_mul_ps (c[2].v.sse, op.v.sse);
  return *this;
}

inline SSEVec3D& SSEVec3D::operator-=(const SSEVec3D& op) {
  c[0].v.sse = _mm_sub_ps (c[0].v.sse, op.c[0].v.sse);
  c[1].v.sse = _mm_sub_ps (c[1].v.sse, op.c[1].v.sse);
  c[2].v.sse = _mm_sub_ps (c[2].v.sse, op.c[2].v.sse);
  return *this;
}

inline  SSEVec3D& SSEVec3D::operator=(const Vector3D& op) {
   c[0] = _mm_set_ps1(op.value[0]);
   c[1] = _mm_set_ps1(op.value[1]);
   c[2] = _mm_set_ps1(op.value[2]);
   return *this;
}

inline  SSEVec3D& SSEVec3D::operator=(const __m128& op) {
   c[0].v.sse = op;
   c[1].v.sse = op;
   c[2].v.sse = op;
   return *this;
}

inline __m128 SSEVec3D::normalizeRL() {
//   __m128 length = _mm_sqrt_ps((*this) * (*this));
//   __m128 sqrLen = (*this) * (*this)
  SSE4 sqrLen ;
  sqrLen.v.sse = (*this) * (*this);
  __m128 length = _mm_set_ps(sqrt(sqrLen.v.f[3]), sqrt(sqrLen.v.f[2]), sqrt(sqrLen.v.f[1]), sqrt(sqrLen.v.f[0]));
  __m128 invLength = _mm_div_ps(ONE, length);
  c[0].v.sse = _mm_mul_ps(c[0].v.sse, invLength);
  c[1].v.sse = _mm_mul_ps(c[1].v.sse, invLength);
  c[2].v.sse = _mm_mul_ps(c[2].v.sse, invLength);
  return length;
}

inline __m128 SSEVec3D::length() const {
  return _mm_sqrt_ps((*this) * (*this));
}

inline void SSEVec3D::setVec(unsigned int i, const Vector3D& v) {
  setVec(i, v.value);
}

inline void SSEVec3D::setVec(unsigned int i, const float* v) {
  c[0].v.f[i] = v[0];
  c[1].v.f[i] = v[1];
  c[2].v.f[i] = v[2];
}

#endif
