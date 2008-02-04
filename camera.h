//
// C++ Interface: camera
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CAMERA_H
#define CAMERA_H

#include "vector3d.h"

/**
A basic camera. Providing information about point of view, projection aera etc..

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Camera
{
  public:
    Camera(const Vector3D& position = Vector3D(0.0, 0.0, -1.0), const Vector3D& target = Vector3D(0.0), const Vector3D& up = Vector3D(0.0, 1.0, 0.0), float d = 1.0, unsigned int x = 320, unsigned int y = 240);
    void setPosition(const Vector3D& position);
    void update();
    ~Camera();

    void setPropertyFromString(const std::string& key, const std::string& value );
    void setMovementSpeed(float speed) { moveForwardSpeed = speed; }
    /**
      Sets the angle speed with which the camera is turning around
      the local y/x-axis.
      @param pitchOrYaw 0 = yaw, 1 = pitch
     **/
    void setTurnSpeed(float speed, unsigned int pitchOrYaw) { angleSpeed[pitchOrYaw] = speed; }
    void ani(float dt = 1.0);
    Vector3D position;
    Vector3D target;
    Vector3D up;
    float d; /// distance from cam to projectionplane


    float projWidth;          ///< Width of the projection plane
    float projHeight;         ///< Height of the projection plane

    Vector3D projOrigin;         ///< Origin (upper left corner) of the projection plane
    Vector3D u;                  ///< Current vector in x direction
    Vector3D v;                  ///< Current vector in y direction
    unsigned int resolution[2];  ///< Resolution of the projection

    // animation parameters
    float moveForwardSpeed;
    float angleSpeed[2];

};

#endif
