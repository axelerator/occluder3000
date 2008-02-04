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
position(0.0), color(RGBvalue(1.0, 1.0, 1.0)), col4(color.getRGB()), direction(0.0, -1.0, 0.0){

}

Light::Light(const Vector3D& pos, const RGBvalue& color):
position(pos), pos4(pos), color(color), col4(color.getRGB()), direction(0.0, -1.0, 0.0) {

}

Light::Light(const Light& l):
position(l.getPosition()), color(l.getColor()), col4(color.getRGB()), direction(l.direction){}

Light::~Light()
{
}




/*!
    \fn Light::setPosition(float x, float y, float z)
 */
void Light::setPosition(float x, float y, float z) {
    position.value[0] = x;
    position.value[0] = y;
    position.value[0] = z;
}
