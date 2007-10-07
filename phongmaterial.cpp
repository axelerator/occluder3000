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
PhongMaterial::PhongMaterial() {

float col = ((float)rand()/RAND_MAX) * 3.0;
DEBUG("color" << col);
if ( col < 1.0) {
DEBUG("red");
  diffuse[0] = col;
  diffuse[1] = 1.0 - col;
  diffuse[2] = 1.0 - col;
} else if ( col < 2.0) {
DEBUG("green");
  diffuse[0] = col - 1.0;
  diffuse[1] = 2.0 - col;
  diffuse[2] = 2.0 - col;
} else {
DEBUG("blue");

  diffuse[0] = col - 2.0;
  diffuse[1] = 3.0 - col;
  diffuse[2] = 3.0 - col;
}
  diffuse[0] = 1.0;
  diffuse[1] = 1.0;
  diffuse[2] = 1.0;

}


PhongMaterial::~PhongMaterial()
{
}


