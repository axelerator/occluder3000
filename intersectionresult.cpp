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

IntersectionResult::IntersectionResult() : intersectedAtAll(false) {
}

IntersectionResult::~IntersectionResult(){
}
IntersectionResult::IntersectionResult(const IntersectionResult& ir):
poi(ir.getPOI()) {}



IntersectionResult& IntersectionResult::operator=(const IntersectionResult& v) {
  memcpy(this, &v, sizeof(IntersectionResult));
  return (*this);
}

const Vector3D& IntersectionResult::calcPOI() {
  this->poi = orig + (u * e1) + (v * e2);
  return poi;
}
