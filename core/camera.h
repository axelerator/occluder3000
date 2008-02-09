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

#include <string>
#include "vec3.h"
namespace Occluder {
    /**
    A basic camera. Providing information about point of view, projection aera etc..

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
    class Camera {
public:
        Camera(const Vec3& position = Vec3(0.0, 0.0, -1.0), const Vec3& target = Vec3(0.0), const Vec3& up = Vec3(0.0, 1.0, 0.0), float d = 1.0, unsigned int x = 320, unsigned int y = 240);
        void update();
        ~Camera();

        /**
          Changes the light based on a confguration string.
          Key contains the property to be set(i.e. "position")
          and value a string that can be converted into the matching
          value (Vec3-> "0.4 -0.234 15.2")
          **/
        void setPropertyFromString(const std::string& key, const std::string& value );
        void setMovementSpeed(float speed) {
            moveForwardSpeed = speed;
        }
        /**
          Sets the angle speed with which the camera is turning around
          the local y/x-axis.
          @param pitchOrYaw 0 = yaw, 1 = pitch
         **/
        void setTurnSpeed(float speed, unsigned int pitchOrYaw) {
            angleSpeed[pitchOrYaw] = speed;
        }
        void ani(float dt = 1.0);

        const Vec3& getV() const;
        const Vec3& getU() const;
        void setTarget(const Vec3& theValue);
        const Vec3& getTarget() const;
        void setResolution(unsigned int width, unsigned int height);
        const unsigned int* getResolution() const;
        const Vec3& getProjcetionplaneOrigin() const;
        void setPosition(const Vec3& theValue);
        const Vec3& getPosition() const;


private:

        Vec3 position;
        Vec3 target;
        Vec3 up;
        float d; /// distance from cam to projectionplane

        Vec3 projOrigin;         ///< Origin (upper left corner) of the projection plane
        Vec3 u;                  ///< Current vector in x direction
        Vec3 v;                  ///< Current vector in y direction
        unsigned int resolution[2];  ///< Resolution of the projection in pixels

        // animation parameters
        float moveForwardSpeed;
        float angleSpeed[2];

    };
}

// ------------------------- implementations of inlined methods --------------------------

inline const Vec3& Occluder::Camera::getPosition() const {
    return position;
}

inline void Occluder::Camera::setPosition(const Vec3& theValue) {
    position = theValue;
}

inline const Vec3& Occluder::Camera::getProjcetionplaneOrigin() const {
    return projOrigin;
}

inline const unsigned int* Occluder::Camera::getResolution() const {
    return resolution;
}

inline void Occluder::Camera::setResolution(unsigned int width, unsigned int height) {
    resolution[0] = width;
    resolution[1] = height;
}

inline const Vec3& Occluder::Camera::getTarget() const {
    return target;
}

inline void Occluder::Camera::setTarget(const Vec3& theValue) {
    target = theValue;
}

inline const Vec3& Occluder::Camera::getU() const {
    return u;
}

inline const Vec3& Occluder::Camera::getV() const {
    return v;
}

#endif
