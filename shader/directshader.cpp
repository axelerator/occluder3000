//
// C++ Implementation: directshader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "directshader.h"
#include "scene.h"
#include "primitive.h"
#include "intersection.h"
#include "light.h"
#include "raysegmentignore.h"
#include "intersectionsse.h"
#include "raysegmentsse.h"

using namespace Occluder;

DirectShader::DirectShader(const std::string& name, const Scene& scene, const Vec3& color)
: Shader(name, scene), color(color) {
}


DirectShader::~DirectShader() {
}


Vec3 DirectShader::getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const {
  const size_t lightCount = scene.getLightCount();
  Vec3 radiance(0.0f);
  for ( size_t i = 0; i < lightCount; ++i) {
    const Light& light = scene.getLight( i );
    Vec3 l( light.getPosition() - intersection.getLocation() );
    const float distance = l.normalizeRL();
    const Vec3& normal = intersection.getNormal();

    const float d1 = fmaxf(normal * l, 0.0f);
    const float d2 = fmaxf( d1 * ((l * -1.0f) * light.getDirection()), 0.0f);
    if ( d2 > 0.0f ) {
      const RaySegment shadowRay( intersection.getLocation() + l * 0.00001, l, 0.0, distance );
//       const RaySegmentIgnore shadowRay( intersection.getLocation(), l, intersection.getPrimitive(), 0.0, distance );
      if ( ! scene.hasIntersection( shadowRay ) )
        radiance += (light.getColor() ^ color) * d2;
    }
  }
  return radiance;
}

Vec3SSE DirectShader::getRadiance( const Vec3SSE& directions,  const IntersectionSSE& intersections, unsigned int depth) const {
  const size_t lightCount = scene.getLightCount();
  Vec3SSE radiance(0.0f);
  const Float4 minusOne(-1.0f);
  const Float4 epsilon(0.0001f);
  for ( size_t i = 0; i < lightCount; ++i) {
    const Light& light = scene.getLight( i );
    const Vec3SSE lightPos4(light.getPosition());
    Vec3SSE l(lightPos4  - intersections.getLocations() );
    const Float4 distance = l.normalizeRL() - epsilon;
    const Vec3SSE& normal = intersections.getNormals();

    const Float4 d1 = max4(normal * l, Float4::zero());
    Float4 d2 = max4( d1 * ((l * minusOne) * light.getDirection()), Float4::zero());
    const Vec3SSE color(this->color);
    const Vec3SSE lightColor(light.getColor());
    const RaySegmentSSE shadowRays(light.getPosition(), l * minusOne, distance);
    const Float4 shadows = scene.haveIntersections( shadowRays );
    d2 &= shadows.andnot(Float4::BINONE);
//     if ( d2 > 0.0f ) {
//       const RaySegment shadowRay( intersection.getLocation() + l * 0.00001, l, 0.0, distance );
//       const RaySegmentIgnore shadowRay( intersection.getLocation(), l, intersection.getPrimitive(), 0.0, distance );
//       if ( ! scene.hasIntersection( shadowRay ) )
        radiance += (lightColor ^ color) * d2;
//     }
  }
  return radiance;

}

void DirectShader::setPropertyFromString(const std::string& key, const std::string& value ) {
  if ( key == "diffuse" )
    color = Vec3( value.c_str());
  else
    Shader::setPropertyFromString(key, value);
}
