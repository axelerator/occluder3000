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

Camera::Camera(Vector3D& position, Vector3D& target, Vector3D& up, float d, unsigned int x, unsigned int y) :
 position(position), target(target), up(up), d(d) {
resolution[0] = x;
resolution[1] = y;
update();
}


Camera::~Camera()
{}

void Camera::setPosition(const Vector3D& position) {
  this->position = position;
update();

}

void Camera::update() {
  Vector3D eyeToTarget = (target - position).normal();

  u = (up % eyeToTarget).normal();
  v = (u % eyeToTarget ).normal();

  // Calculate the center of the projectionplane
  Vector3D  pplaneCenter = position + (eyeToTarget * d);
  projOrigin = pplaneCenter - u - v;
  u *= 2.0;
  v *= 1.75;
}

