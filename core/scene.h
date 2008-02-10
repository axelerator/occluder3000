//
// C++ Interface: scene
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCENE_H
#define SCENE_H

#include <map>

#include "list.h"
#include "vec3.h"
#include "accelerationstructure.h"
#include "primitive.h"
#include "aabb.h"
#include "camera.h"
#include "directshader.h"

namespace Occluder {
class Intersection;
class RaySegment;
class Light;
/**
Contains all information about the scene, like camera, lights and geometry.

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Scene {
public:
    Scene();
    ~Scene();
    /**
      Loads a scene from the given file.
      @param filename of the file containing a simple ascii based scene description
      @return false if loading did not succeed
    **/
    const bool loadFromFile ( const std::string& filename );

    /**
        @param i index of the requested primitive
        @return reference to the primitive in the scene
     **/
    const Primitive& getPrimitive(size_t i) const;

    /**
      @return number of primitives stored in scene
     **/
    size_t getPrimitiveCount() const;

    /**
      adds a vertex to the scene.
      @param the vertex that is to be added
      @return the index of the inserted vertex
     **/
    size_t addVertex(const Vec3& v);

    /**
      @param i listindex of the requested vertex
      @return Vertex with index i
     **/
    const Vec3& getVertex(const size_t i) const;

    /**
      @return the total amount of vertices in this scene
     **/
    size_t getVertexCount();

    /**
      adds a primitive to the scene
      @param the primitive that is to be added
      @return index of the primitive
     **/
    size_t addPrimitive(const Primitive& prim);

    /**
      @return the camera the scene should be rendered from
    **/
    const Camera& getCamera() const;

    /**
      Determines the point where a ray intersect the first time
      an object of the scene.
      @param the ray in question
      @return The point and other parameters of the intersection @see Intersection
    **/
    const Intersection trace(const RaySegment& ray) const;

    /**
      Determines if the raysegment is intersected by any primitiv.
      Primararily used for shadow rays. Calls AccelerationStructure#hasIntersection
      @return true if the ray hit a primitive between tmin and tmax.
    **/
    bool hasIntersection(const RaySegment& ray ) const;

    /**
      Determines if the raysegment is intersected by any primitiv.
      Primararily used for shadow rays. Calls AccelerationStructure#hasIntersection
      @return true if the ray hit a primitive between tmin and tmax.
    **/
    bool hasIntersection(const RaySegmentIgnore& ray ) const;

    const Shader& getShader(const std::string& name) const ;

    /**
      @return the total amount of lights in this scene.
     **/
    size_t getLightCount() const;

    /**
      @param idx list index of the requested light source
      @return the lightsource
     **/
    const Light& getLight(size_t idx) const;

private:
    enum ObjectType {NONE, LIGHT, MATERIAL, CAMERA};


    List<Primitive> primitives;
    List<Vec3> vertices;
    std::map<std::string, const Shader*> shaderMap;
    std::vector<Shader*> shader;
    std::vector<Light*> lights;

    AABB aabb;
    AccelerationStructure *geometry;
    const DirectShader defaultShader;// cannot be static since shader needs ref to a scene
    Camera cam;


};
}


//  ------------------ implementation of inlined methods ------------------
using namespace Occluder;

inline const Primitive& Scene::getPrimitive(size_t i) const {
  return primitives[i];
}

inline size_t Scene::getPrimitiveCount() const {
  return primitives.size();
}


inline const Vec3& Scene::getVertex(const size_t i) const {
  return vertices[i];
}

inline const Camera& Scene::getCamera() const {
  return cam;
}

inline size_t Scene::getLightCount() const {
  return lights.size();
}

inline const Light& Scene::getLight(size_t idx) const {
  return *(lights[idx]);
}

inline bool Scene::hasIntersection(const RaySegment& ray ) const {
  return geometry->hasIntersection(ray);
}

inline bool Scene::hasIntersection(const RaySegmentIgnore& ray ) const {
  return geometry->hasIntersection(ray);
}
#endif
