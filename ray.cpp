//
// C++ Implementation: ray
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ray.h"

Ray::Ray(){}

Ray::Ray(const Vector3D& s, const Vector3D& d) :
start(s), direction(d) {}


Ray::~Ray()
{}


