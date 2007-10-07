//
// C++ Interface: light
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LIGHT_H
#define LIGHT_H
#include "vector3d.h"
#include "rgbvalue.h"
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Light{
public:
    Light();
    Light(const Vector3D& pos, const RGBvalue& color);
    Light(const Light& l);
    const Vector3D& getPosition() const { return position;}
    const RGBvalue& getColor() const {return color; }
    ~Light();
private:
    Vector3D position;
    RGBvalue color;
};

#endif
