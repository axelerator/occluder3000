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
#include "rgbvalue.h"

PhongMaterial::PhongMaterial(float r,float g,float b,float alpha,float refract,float reflection) :
 alpha(alpha), refract(refract), reflection(reflection)/*, dif4(Vector3D(r,g,b))*/ {
diffuse[0] = r;
diffuse[1] = g;
diffuse[2] = b;

}


PhongMaterial::~PhongMaterial() {}

void PhongMaterial::setPropertyFromString(const std::string& key, const std::string& value) {
  if ( key == "diffuse") {
    RGBvalue dif = RGBvalue(value);
    diffuse[0] = dif.getRGB()[0];
    diffuse[1] = dif.getRGB()[1];
    diffuse[2] = dif.getRGB()[2];
  } else if ( key == "reflection") {
    reflection = atof(value.c_str());
  } else if ( key == "alpha") {
    alpha = atof(value.c_str());
  } else if ( key == "refraction") 
    refract = atof(value.c_str());
   else 
    std::cerr << "unknown property for light: " << key << std::endl;
}

PhotonBehavior PhongMaterial::spinRoulette() const {
    const float spin =  (rand() / (RAND_MAX + 1.0)) * (1.0) ;
    if ( spin < alpha ) {
      if ( spin < (reflection * alpha)) {
        return SPEC_REFLECT;
      } else {
        const float avg = (diffuse[0]+diffuse[1]+diffuse[2]) * .33333f * alpha * (1.0f - reflection);
        if ( spin < avg ) 
          return ABSORB;
        else
          return DIF_REFLECT;
      }
    } else
      return TRANSMIT;
}
