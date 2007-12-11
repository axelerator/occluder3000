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
    Camera(Vector3D& position, Vector3D& target, Vector3D& up, float d, unsigned int x, unsigned int y);
    void setPosition(const Vector3D& position);
    void update();
    ~Camera();

    
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

};

#endif
