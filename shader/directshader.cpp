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

using namespace Occluder;

DirectShader::DirectShader(const Vec3& color, const Scene& scene)
: Shader(scene), color(color) {
}


DirectShader::~DirectShader() {
}


Vec3 DirectShader::getRadiance(const Vec3& direction, const Intersection& intersection) const {
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
//       const RaySegment shadowRay( intersection.getLocation() + l * 0.00001, l, 0.0, distance );
      const RaySegmentIgnore shadowRay( intersection.getLocation(), l, intersection.getPrimitive(), 0.0, distance );
      if ( ! scene.hasIntersection( shadowRay ) )
        radiance += (light.getColor() ^ color) * d2;
    }
  }
  return radiance;
}


