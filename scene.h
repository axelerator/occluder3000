//
// C++ Interface: scene
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <map>
#include <string>
#include "light.h"
#include "phongmaterial.h"
#include "assert.h"
#include "photonmap.h"
#include "camera.h"

class AccelerationStruct;
class PhotonMap;
class RadianceRay;

enum ObjectType {NONE, LIGHT, MATERIAL, CAMERA};

/**
Container for geomtrydatastruct and other objects (lights, cams, etc)

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class Scene{
public:
    Scene();
    const bool loadFromFile( const std::string& filename);
    void addMaterial(const std::string& name, const PhongMaterial& mat) {
      material.insert(std::pair<std::string, PhongMaterial>(name, mat));
    }
    PhongMaterial& getMaterial(const std::string& name);
    void addLight(const Light& l);
    Light &addLight() { lights.push_back(Light()); return lights.back();}
    const std::vector<Light> &getLights() const { return lights; }
    Light &getLight(int i) { return lights[i]; }
    unsigned int getLightCount() { return lights.size(); }
    void setGeometry(AccelerationStruct *geometry) {assert(geometry != 0 ); this->geometry = geometry;}
    AccelerationStruct &getGeometry() const { return *geometry; }
    void updatePhotonMap();
    void recievePhoton ( RadianceRay& photon, const Vector3D& power, unsigned int depth );
    void recieveCausticPhoton ( RadianceRay& photon, const Vector3D& power, unsigned int depth );
    const Camera &getCamera() const { return cam; }
    Camera &getCamera() { return cam; }
    ~Scene();

private:
  std::vector<Light> lights;
  Camera cam;
  AccelerationStruct *geometry;
  std::map<std::string, PhongMaterial> material;
  PhongMaterial defaultMaterial;

  std::string geometryfile;

  PhotonMap *photonMap;
  PhotonMap causticsMap;

};

#endif
