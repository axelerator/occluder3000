//
// C++ Implementation: accelerationstruct
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "accelerationstruct.h"

AccelerationStruct::AccelerationStruct(const Scene& scene) : scene ( scene )  {}


AccelerationStruct::~AccelerationStruct() {}

void AccelerationStruct::setBounds(float* newBounds) {
  memcpy(this->bounds, newBounds, 6 * sizeof(float));
  this->boundsSet = true;
}

void AccelerationStruct::addTriangle ( const Triangle& t ) {
    triangles.push_back ( t );
}

