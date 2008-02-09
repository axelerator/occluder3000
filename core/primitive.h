//
// C++ Interface: primitiv
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PRIMITIV_H
#define PRIMITIV_H

#include "vec3.h"
#include "shader.h"


namespace Occluder {
    class Scene;

    /**
      A geometric object of the rendered scene. Is now just a triangle,
      and implemented in place to avoid unnecessary virtual binding.

    	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
    */
    class Primitive {
public:
        Primitive(unsigned int p0, unsigned int p1, unsigned int p2, const Scene& scene, const std::string& shaderName);
        Primitive& operator=(const Primitive& op);
        ~Primitive();

        /**
          Tests this primitive and a ray for intersection.
          @param r the ray the primitive ist to be tested with
          @return true if the ray interescts this primitive
         **/
        bool intersects(const RaySegment& r) const;

        /**
          Determins the intersection of this primitive and a ray.
          @param r the ray the primitive ist to be tested with
          @return The intersection, or the 'empty' intersection if no
                  intersection occurs. @see Intersection#getEmpty
         **/
        const Intersection getIntersection( const RaySegment& r) const;

        /**
          @return the shader aka material of this primitive
         **/
        const Shader& getShader() const;

        /**
          @return a reference to the normal of this primitive.
                  In the current special case of flat primitives this can
                  be a reference. For more complex primitives this might
                  have to be changed to a value in dependency of u and v.
         **/
        const Vec3& getNormal() const;
    
private:
        const unsigned int p0, p1, p2;
        const Vec3 u,v;
        const Vec3 normal;

        const Scene& scene;
        const Shader &shader;
    };
}


// ------------------ implementations for inlined methods ------------------
using namespace Occluder;

inline Primitive& Primitive::operator=(const Primitive& op) {
    memcpy(this, &op, sizeof(Primitive));
    return *this;
}

inline const Shader &Primitive::getShader() const {
    return shader;
}
 
inline const Vec3& Primitive::getNormal() const {
  return normal;
}

#endif
