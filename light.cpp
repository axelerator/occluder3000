//
// C++ Implementation: light
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "light.h"

Light::Light():
position(0.0), color(RGBvalue(1.0, 1.0, 1.0)){

}

Light::Light(const Vector3D& pos, const RGBvalue& color):
position(pos), color(color) {

}

Light::Light(const Light& l):
position(l.getPosition()), color(l.getColor()){}

Light::~Light()
{
}

