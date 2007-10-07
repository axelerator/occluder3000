//
// C++ Implementation: intersectionresult
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "intersectionresult.h"

IntersectionResult::IntersectionResult(bool didInterersect):
didInterersect(didInterersect)
{
}

IntersectionResult::IntersectionResult(const IntersectionResult& ir):
didInterersect(ir.intersect()), poi(ir.getPOI()) {}


IntersectionResult::~IntersectionResult()
{
}

IntersectionResult& IntersectionResult::operator=(const IntersectionResult& v) {
  this->didInterersect = v.intersect();
  this->poi = v.getPOI();
  return (*this);
}

const Vector3D& IntersectionResult::calcPOI() {
  this->poi = orig + (u * e1) + (v * e2);
  return poi;
}
