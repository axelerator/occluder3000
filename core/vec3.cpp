//
// C++ Implementation: vec3
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "vec3.h"
#include "stringutil.h"
#include "assert.h"
using namespace Occluder;

Vec3::~Vec3()
{
}

Vec3::Vec3 (const std::string& valuestr) {
  const std::string trimmed(StringUtil::trim(valuestr));
  const size_t firstSpace = trimmed.find_first_of(" ");
  const size_t secondSpace = trimmed.find_last_of(" ");
  const std::string v1(trimmed.substr(0, firstSpace));
  const std::string v2(trimmed.substr(firstSpace + 1, secondSpace - firstSpace - 1));
  const std::string v3(trimmed.substr(secondSpace + 1));
  c[0] = atof(v1.c_str());
  c[1] = atof(v2.c_str());
  c[2] = atof(v3.c_str());
}



Vec3 Vec3::getRandomSphereVec() {
      assert(false);
      float angle1 = (rand() / (RAND_MAX + 1.0f))* 2.0f * M_PI;
      float angle2 = /*(rand() / (RAND_MAX + 1.0))**/ 2.0f * M_PI;
/*      return Vec3(  cos(angle1)*sin(angle2),
                        cos(angle2),
                        sin(angle1)*sin(angle2));*/
        return Vec3( cos(angle1) * sin(angle2),  cos(angle2), sin(angle1)) * sin(angle2);
}



std::ostream& Occluder::operator << (std::ostream& os, const Vec3& v) {
  return os << "value[0]: " << v.c[0] << " value[1]: " << v.c[1] << " value[2]: " << v.c[2] << std::endl;
}

