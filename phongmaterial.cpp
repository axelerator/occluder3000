//
// C++ Implementation: phongmaterial
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "phongmaterial.h"
#include <stdlib.h>
#include "debug.h"
PhongMaterial::PhongMaterial(float r,float g,float b,float alpha,float refract,float reflection) :
 alpha(alpha), refract(refract), reflection(reflection)/*, dif4(Vector3D(r,g,b))*/ {
diffuse[0] = r;
diffuse[1] = g;
diffuse[2] = b;

}


PhongMaterial::~PhongMaterial()
{
}


