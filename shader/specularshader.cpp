//
// C++ Implementation: specularshader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "specularshader.h"
#include "scene.h"

using namespace Occluder;

SpecularShader::SpecularShader(const std::string& name, const Scene& scene, const Vec3& color, const float reflectivity,  const float alpha, const float refIdx ):
DirectShader( name, scene, color ),
reflectivity(reflectivity) {
}


SpecularShader::~SpecularShader() {
}


Vec3 SpecularShader::getRadiance(const Vec3& direction, const Intersection& intersection, unsigned int depth) const {
    if ( !depth)
      return DirectShader::getRadiance(direction, intersection, 0) * alpha;
    const Vec3 direct( (reflectivity < 1.0f)? DirectShader::getRadiance(direction, intersection, depth) : 0.0f);

    // compute reflection

    const Vec3 reflectedDirection( direction.reflect( intersection.getNormal() ) );
    const RaySegment reflectedRay( intersection.getLocation() + 0.00001f * reflectedDirection,  reflectedDirection);
    const Intersection reflectedIntersection = scene.trace( reflectedRay );
    const Vec3 reflectedIrradiance( ( reflectedIntersection.isEmpty() ) ? (0.0f) :  reflectedIntersection.getRadiance( reflectedDirection, depth - 1 ) );


    // compute transmitted radiance
    Vec3 transmitted(0.0f);
    if ( alpha < 1.0f) {
        Vec3 refrDirIn( direction.refract( intersection.getNormal(), 1.0f, refIdx ) );
        RaySegment reflectedRay( intersection.getLocation() + 0.00001f * refrDirIn, refrDirIn );
    
    
        const Intersection inters = scene.trace( reflectedRay );
    
        if ( !inters.isEmpty() ) {
            Vec3 refrDirOut = refrDirIn.refract( inters.getNormal() * -1.0, refIdx, 1.0);
    
            RaySegment transmittedRay(
                inters.getLocation() + + 0.00001f * refrDirOut, refrDirOut);
            const Intersection transI = scene.trace( transmittedRay );
            if ( !transI.isEmpty() )
              transmitted = ( transI.getRadiance( refrDirOut, depth - 1 ) * ( 1.0f - alpha) );
        }
    }
    return (direct * ( 1.0f - reflectivity) + reflectedIrradiance * reflectivity) * alpha
           + transmitted  ;
}

void SpecularShader::setPropertyFromString(const std::string& key, const std::string& value ) {
    if ( key == "reflection" )
        reflectivity = atof( value.c_str());
    else if ( key == "alpha" )
        alpha = atof( value.c_str());
    else if ( key == "refraction" )
        refIdx = atof( value.c_str());
    else
        DirectShader::setPropertyFromString(key, value);
}
