//
// C++ Interface: light
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"

namespace Occluder {

    /**
    A Basic light source in the scene

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
    class Light {
public:
        Light();

        ~Light();

        void setColor(const Vec3& theValue);
        const Vec3& getColor() const;
        void setDirection(const Vec3& theValue);
        const Vec3& getDirection() const;
        void setPosition(const Vec3& theValue) ;
        const Vec3& getPosition() const;
        /**
          Changes the light based on a confguration string.
          Key contains the property to be set(i.e. "position")
          and value a string that can be converted into the matching
          value (Vec3-> "0.4 -0.234 15.2")
         **/
        void setPropertyFromString(const std::string& key, const std::string& value);

protected:
        Vec3 direction;
        Vec3 color;
        Vec3 position;
    };
}


// ---------------------------- implementation of inlined methods -------------------
using namespace Occluder;

inline void Light::setColor(const Vec3& theValue) {
    color = theValue;
}

inline const Vec3& Light::getColor() const {
    return color;
}

inline void Light::setDirection(const Vec3& theValue) {
    direction = theValue;
}

inline const Vec3& Light::getDirection() const {
    return direction;
}

inline void Light::setPosition(const Vec3& theValue) {
    position = theValue;
}

inline const Vec3& Light::getPosition() const {
    return position;
}

#endif
