//
// C++ Interface: Float4
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
#include "vec3.h"

namespace Occluder {
/**
Wrapper for SSE operations

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Float4 {
  public:
    Float4(){}
    Float4(const Float4& o)   { memcpy(this->v.f, o.v.f, sizeof(o));}
    Float4(const __m128 d)  { v.sse = d;}
    Float4(const float f)   { v.sse = _mm_set_ps1(f);}
    Float4 operator+(const Float4& rhs) const;
    Float4 operator*(const Float4& rhs) const;
    Float4 operator/(const Float4& rhs) const;
    Float4 operator-(const Float4& rhs) const;

    Float4 operator<(const Float4& op) const;
    Float4 operator<=(const Float4& op) const;
    Float4 operator>(const Float4& op) const;
    Float4 operator>=(const Float4& op) const;
    Float4 operator|(const Float4& op) const;

    Float4 andnot(const Float4& op) const;
    Float4 operator&(const Float4& op) const;
    Float4& operator&=(const Float4& op);
    Float4 sqrt() const;
    int getMask() const;

    int operator==(int op) const;
    static Float4 zero();

    ~Float4(){}
    static const Float4 EPSILON4;
    static const Float4 EPSILON4_NEG;
    static const Float4 ONE;
    static const Float4 BINONE;
  
  union {
    float f[4];
    __m128 sse;
  } v;

};


class Vec3SSE {
  public:
  Vec3SSE(){}
  Vec3SSE(const Vec3& v){
   c[0] = _mm_set_ps1(v[0]);
   c[1] = _mm_set_ps1(v[1]);
   c[2] = _mm_set_ps1(v[2]);
  }
  Vec3SSE(const float* v){
   c[0] = _mm_set_ps1(v[0]);
   c[1] = _mm_set_ps1(v[1]);
   c[2] = _mm_set_ps1(v[2]);
  } 

  Vec3SSE(const Vec3& q, const Vec3& u, const Vec3& v );

  static void cross(const Vec3SSE& a, const Vec3SSE& b, Vec3SSE& dest);
  Vec3SSE operator%(const Vec3SSE& b) const;
  Float4 operator*(const Vec3SSE& b) const;
  Vec3SSE operator*(const Float4& b) const;
  Vec3SSE operator^(const Vec3SSE& b) const;
  Vec3SSE operator-(const Vec3SSE& r) const;
  Vec3 get(unsigned int i) const;
  Vec3SSE& operator+=(const Vec3SSE& op);
  Vec3SSE& operator*=(const Float4& op);
  Vec3SSE& operator-=(const Vec3SSE& op);
  Vec3SSE& operator=(const Vec3& op);
  Vec3SSE& operator=(const __m128& op);



  Float4 normalizeRL();
  Float4 length() const;
  void setVec(unsigned int i, const Vec3& v);
  void setVec(unsigned int i, const float *v);
  
  Float4 c[3];
};


// -------------------------- implementations of inlined methods --------------------------------


inline Float4 Float4::operator+(const Float4& rhs) const {
  return _mm_add_ps(this->v.sse, rhs.v.sse);
}

inline Float4 Float4::operator*(const Float4& rhs) const {
  return _mm_mul_ps(this->v.sse, rhs.v.sse);
}

inline Float4 Float4::operator-(const Float4& rhs) const {
  return _mm_sub_ps(this->v.sse, rhs.v.sse);
}

inline Float4 Float4::operator/(const Float4& rhs) const {
  return _mm_div_ps(this->v.sse, rhs.v.sse);
}
inline Float4 Float4::operator<(const Float4& op) const {
  return Float4(_mm_cmplt_ps(this->v.sse, op.v.sse));
}

inline Float4 Float4::operator<=(const Float4& op) const {
  return Float4(_mm_cmple_ps(this->v.sse, op.v.sse));
}

inline Float4 Float4::andnot(const Float4& op) const {
  return Float4(_mm_andnot_ps(this->v.sse, op.v.sse));
}


inline Float4 Float4::operator>(const Float4& op) const {
  return Float4(_mm_cmpgt_ps(this->v.sse, op.v.sse));
}

inline Float4 Float4::operator|(const Float4& op) const {
  return Float4(_mm_or_ps(this->v.sse, op.v.sse));
}

inline Float4 Float4::operator&(const Float4& op) const {
  return Float4(_mm_and_ps(this->v.sse, op.v.sse));
}

inline Float4& Float4::operator&=(const Float4& op){
  v.sse = _mm_and_ps(this->v.sse, op.v.sse);
  return *this;
}

inline int Float4::operator==(int op) const {
  return _mm_movemask_ps(this->v.sse) == op;
}

inline Float4 Float4::zero() {
  return Float4(_mm_setzero_ps());
}

inline Float4 Float4::sqrt() const {
 return Float4( _mm_sqrt_ps( this->v.sse) );
}

inline int Float4::getMask() const {
 return _mm_movemask_ps( this->v.sse );
}

inline Vec3SSE::Vec3SSE(const Vec3& q, const Vec3& u, const Vec3& v ){
    c[0].v.sse = _mm_set_ps1( q[0] );
    c[1].v.sse = _mm_set_ps1( q[1] );
    c[2].v.sse = _mm_set_ps1( q[2] );

    c[0].v.sse = _mm_add_ps(c[0].v.sse , _mm_set_ps(u[0] + v[0], v[0], u[0], 0.0));
    c[1].v.sse = _mm_add_ps(c[1].v.sse , _mm_set_ps(u[1] + v[1], v[1], u[1], 0.0));
    c[2].v.sse = _mm_add_ps(c[2].v.sse , _mm_set_ps(u[2] + v[2], v[2], u[2], 0.0)); 

}

inline void Vec3SSE::cross(const Vec3SSE& a, const Vec3SSE& b, Vec3SSE& dest) {
  dest.c[0].v.sse = _mm_sub_ps( _mm_mul_ps(a.c[1].v.sse, b.c[2].v.sse), _mm_mul_ps(a.c[2].v.sse, b.c[1].v.sse));
  dest.c[1].v.sse = _mm_sub_ps( _mm_mul_ps(a.c[2].v.sse, b.c[0].v.sse), _mm_mul_ps(a.c[0].v.sse, b.c[2].v.sse));
  dest.c[2].v.sse = _mm_sub_ps( _mm_mul_ps(a.c[0].v.sse, b.c[1].v.sse), _mm_mul_ps(a.c[1].v.sse, b.c[0].v.sse));
}

inline Vec3SSE Vec3SSE::operator%(const Vec3SSE& b) const {
  Vec3SSE result;
  result.c[0].v.sse = _mm_sub_ps( _mm_mul_ps(this->c[1].v.sse, b.c[2].v.sse), _mm_mul_ps(this->c[2].v.sse, b.c[1].v.sse));
  result.c[1].v.sse = _mm_sub_ps( _mm_mul_ps(this->c[2].v.sse, b.c[0].v.sse), _mm_mul_ps(this->c[0].v.sse, b.c[2].v.sse));
  result.c[2].v.sse = _mm_sub_ps( _mm_mul_ps(this->c[0].v.sse, b.c[1].v.sse), _mm_mul_ps(this->c[1].v.sse, b.c[0].v.sse));
  return result;
}

inline Float4 Vec3SSE::operator*(const Vec3SSE& b) const {
  return _mm_add_ps( _mm_add_ps(_mm_mul_ps(c[0].v.sse, b.c[0].v.sse) ,
                              _mm_mul_ps(c[1].v.sse, b.c[1].v.sse) ),
                              _mm_mul_ps(c[2].v.sse, b.c[2].v.sse) );
}

inline Vec3SSE Vec3SSE::operator*(const Float4& b) const {
  Vec3SSE result;
  result.c[0].v.sse = _mm_mul_ps (this->c[0].v.sse, b.v.sse);
  result.c[1].v.sse = _mm_mul_ps (this->c[1].v.sse, b.v.sse);
  result.c[2].v.sse = _mm_mul_ps (this->c[2].v.sse, b.v.sse);
  return result;
}

/**
  Componentwise multiplication
**/
inline Vec3SSE Vec3SSE::operator^(const Vec3SSE& b) const {
  Vec3SSE result;
  result.c[0].v.sse = _mm_mul_ps(this->c[0].v.sse, b.c[0].v.sse);
  result.c[1].v.sse = _mm_mul_ps(this->c[1].v.sse, b.c[1].v.sse);
  result.c[2].v.sse = _mm_mul_ps(this->c[2].v.sse, b.c[2].v.sse);
  return result;
}

inline Vec3SSE Vec3SSE::operator-(const Vec3SSE& r) const {
  Vec3SSE result;
  result.c[0].v.sse = _mm_sub_ps (this->c[0].v.sse, r.c[0].v.sse);
  result.c[1].v.sse = _mm_sub_ps (this->c[1].v.sse, r.c[1].v.sse);
  result.c[2].v.sse = _mm_sub_ps (this->c[2].v.sse, r.c[2].v.sse);
  return result;
}

inline Vec3 Vec3SSE::get(unsigned int i) const {
  return Vec3 (c[0].v.f[i],c[1].v.f[i],c[2].v.f[i] );
}


inline Vec3SSE& Vec3SSE::operator+=(const Vec3SSE& op) {
  c[0].v.sse = _mm_add_ps (c[0].v.sse, op.c[0].v.sse);
  c[1].v.sse = _mm_add_ps (c[1].v.sse, op.c[1].v.sse);
  c[2].v.sse = _mm_add_ps (c[2].v.sse, op.c[2].v.sse);
  return *this;
}

inline Vec3SSE& Vec3SSE::operator*=(const Float4& op) {
  c[0].v.sse = _mm_mul_ps (c[0].v.sse, op.v.sse);
  c[1].v.sse = _mm_mul_ps (c[1].v.sse, op.v.sse);
  c[2].v.sse = _mm_mul_ps (c[2].v.sse, op.v.sse);
  return *this;
}

inline Vec3SSE& Vec3SSE::operator-=(const Vec3SSE& op) {
  c[0].v.sse = _mm_sub_ps (c[0].v.sse, op.c[0].v.sse);
  c[1].v.sse = _mm_sub_ps (c[1].v.sse, op.c[1].v.sse);
  c[2].v.sse = _mm_sub_ps (c[2].v.sse, op.c[2].v.sse);
  return *this;
}

inline  Vec3SSE& Vec3SSE::operator=(const Vec3& op) {
   c[0] = _mm_set_ps1(op[0]);
   c[1] = _mm_set_ps1(op[1]);
   c[2] = _mm_set_ps1(op[2]);
   return *this;
}

inline  Vec3SSE& Vec3SSE::operator=(const __m128& op) {
   c[0].v.sse = op;
   c[1].v.sse = op;
   c[2].v.sse = op;
   return *this;
}

inline Float4 Vec3SSE::normalizeRL() {
  const Float4 leng = length();
  const Float4 invLength = Float4::ONE / leng;
  c[0] = c[0] * invLength;
  c[1] = c[1] * invLength;
  c[2] = c[2] * invLength;
  return leng;
}

inline Float4 Vec3SSE::length() const {
  return ((*this) * (*this)).sqrt();
}

inline void Vec3SSE::setVec(unsigned int i, const Vec3& v) {
  c[0].v.f[i] = v[0];
  c[1].v.f[i] = v[1];
  c[2].v.f[i] = v[2];
}

inline void Vec3SSE::setVec(unsigned int i, const float* v) {
  c[0].v.f[i] = v[0];
  c[1].v.f[i] = v[1];
  c[2].v.f[i] = v[2];
}





}
#endif
