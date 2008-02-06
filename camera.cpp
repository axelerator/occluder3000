//
// C++ Implementation: camera
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "camera.h"

Camera::Camera(const Vector3D& position, const Vector3D& target, const Vector3D& up, float d, unsigned int x, unsigned int y) :
 position(position), target(target), up(up), d(d), moveForwardSpeed(0.0) {
resolution[0] = x;
resolution[1] = y;
angleSpeed[0] = 0.0;
angleSpeed[1] = 0.0;
update();
}


Camera::~Camera()
{}

void Camera::setPosition(const Vector3D& position) {
  this->position = position;
  update();
}

void Camera::update() {
  const Vector3D eyeToTarget = (target - position).normal();
  target = position + eyeToTarget;
  u = (eyeToTarget % up).normal();
  v = (eyeToTarget % u).normal() ;

  // Calculate the center of the projectionplane
  Vector3D  pplaneCenter = position + (eyeToTarget * d);
  v *= (float)(resolution[1])/resolution[0];
  projOrigin = pplaneCenter - u - v;
  u *= 2.0;
  v *= 2.0;
}



void Camera::setPropertyFromString(const std::string& key, const std::string& value ) {
  if ( key == "position" )
    position = Vector3D(value);
  else if ( key == "target" )
    target = Vector3D(value);
  else if ( key == "lookup" )
   up = Vector3D(value);
  else if ( key == "lens" )
   d = atof( value.c_str());
  else if ( key == "resolution" ) {
    const size_t xpos = value.find_first_of("x");
    resolution[0] = atoi( value.substr(0, xpos).c_str());
    resolution[1] = atoi( value.substr(xpos + 1).c_str());
  } else 
    std::cerr << "unknown property for camera: " << key << std::endl;
  update();
}

void Camera::ani(float dt) {
  const Vector3D eyeToTarget((target - position).normal() );
  const Vector3D movement( eyeToTarget * (moveForwardSpeed * dt ) );
  // u is expected to be normalized 
  const Vector3D eyeToNewTarget( (cos(angleSpeed[1]) * ((sin( angleSpeed[0] ) * u) + cos( angleSpeed[0]) * eyeToTarget)) + sin(angleSpeed[1]) * v.normal());
  target = position + eyeToNewTarget + movement;
  position += movement;
  update();
}
