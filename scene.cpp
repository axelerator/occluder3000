//
// C++ Implementation: scene
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <fstream>
#include <iostream>
#include <boost/regex.hpp>

#include "scene.h"
#include "photonmap.h"
#include "radianceray.h"
#include "stringutil.h"
#include "trianglelist.h"
#include "regulargrid.h"
#include "camera.h"
#include "objectloader.h"
//#define DEBUG_ENABLE
#include "bih2.h"
#include "kdtree.h"


#define SPHERE_SAMPLE 300
#define PHOTON_COUNT (SPHERE_SAMPLE*SPHERE_SAMPLE)


Scene::Scene() :geometry(0), defaultMaterial ( 1.0, 1.0, 1.0, 1.0, 0.0, 0.0 ), causticsMap ( PHOTON_COUNT * 3 ) {
  photonMap = new PhotonMap ( PHOTON_COUNT * 3 );
}


const bool Scene::loadFromFile ( const std::string& filename ) {
  std::vector<std::string> file;
  std::string line;
  file.clear();
  std::ifstream infile ( filename.c_str(), std::ios_base::in );
  boost::regex reCfgLine;
  boost::regex reSctLine;
  std::string reStrConfigLine = "(\\w+)\\s*=\\s*(.*)(#.*)?";
  std::string reStrSectionLine = "\\[(\\w+)\\](#.*)?";

  try  {
    reCfgLine.assign ( reStrConfigLine );
    reSctLine.assign ( reStrSectionLine );
  } catch ( boost::regex_error& e ) {
    std::cout << "not a valid regular expression" <<std:: endl;
  }

  if ( infile.fail() )
    return false;
  while ( getline ( infile, line, '\n' ) )  {
    file.push_back ( StringUtil::trim ( line ) );
  }

  // parse global scene settings
  std::vector<std::string>::iterator it = file.begin();
  boost::cmatch matches;
  unsigned int linenr = 0;
  while ( ( it != file.end() && ( ( *it ) [0]  != '[' ) ) ) {
    const std::string& line = ( *it ) ;
    if ( boost::regex_match ( line.c_str(), matches, reCfgLine ) ) {
      const std::string key ( matches[1].first, matches[1].second );
      const std::string value ( matches[2].first, matches[2].second );
      if ( key == "acceleration" ) {
        if ( geometry )
          delete geometry;
        if ( value == "grid" )
          geometry = new RegularGrid ( *this, 0.2 );
        else if ( value == "bih" )
          geometry = new BIH ( *this );
        else if ( value == "kd" )
          geometry = new KdTree ( *this );
        else {
          std::cout << "Accelerationstruct " << value << " not available. Falling back to default(list). Choose { grid, bih, kd }" << std::endl;
          geometry = new Trianglelist ( *this );
        }
      } else if ( key == "geometry" ) {
        std::string geometryfile;
        if ( filename.find ( "/" ) != std::string::npos )
          geometryfile = filename.substr ( 0, filename.find_last_of ( "/" ) +1 )  + value;
        else
          geometryfile = value;

        if ( geometryfile.find ( ".obj" ) != std::string::npos )
          ObjectLoader::loadOBJ ( geometryfile, *this );
        else if ( geometryfile.find ( ".ra2" ) != std::string::npos )
          ObjectLoader::loadRA2 ( geometryfile, *this );
        else
          std::cerr << "Unable to recognize file extension of geometry file: " << geometryfile << std::endl;
      } else if ( key == "resoultion" ) {
        ;
      } else
        std::cout << "Unknown parameter: " << key << std::endl;

    } else if ( ( line == "" ) || ( line[0] == '#' ) ) {
      ;// skip empty lines and comments
    } else {
      std::cerr << "Error parsing line " << linenr << ": " << line << std::endl;
    }

    ++it; // proceed to next line in file
    ++linenr;
  }

  if ( it == file.end() )
    return true;



  // parse scene objects (camera, lights)
  ObjectType currentObject = NONE;
  do {
    const std::string& line = ( *it ) ;
    if ( boost::regex_match ( line.c_str(), matches, reSctLine ) ) {
      const std::string value ( matches[1].first, matches[1].second );
      if ( value == "camera" )
        currentObject = CAMERA;
      else if ( value == "light" )
        currentObject = LIGHT;
      else
        std::cerr << "Unknown object type " << value << " in line " << linenr  << std::endl;
    } else {
      std::cerr << "Error parsing line " << linenr << ": " << line << std::endl;
      return false;
    }
    ++it; // proceed to next line in file
    ++linenr;

    while ( ( it != file.end() ) && ( *it ) [0]  != '[' ) {
    const std::string& line = ( *it ) ;
      if ( boost::regex_match ( line.c_str(), matches, reCfgLine ) ) {
        const std::string value ( matches[2].first, matches[2].second );
        const std::string key ( matches[1].first, matches[1].second );
        switch ( currentObject ) {
          case CAMERA: cam.setPropertyFromString ( key, value ); break;
          case LIGHT:  /*(light.setPropertyFromString(key, value))*/; break;
          default: std::cerr << "No object that " << line << " coud be assigned to, in line " << linenr << std::endl;
        }
      } else if ( ( line == "" ) || ( line[0] == '#' ) ) {
        ;// skip empty lines and comments
      } else {
        std::cerr << "Error parsing line " << linenr << ": " << line << " (skipping line)" << std::endl;
      }
    ++it; // proceed to next line in file
    ++linenr;
    }
  } while ( ( it != file.end() ) );

//       else if ( ( arg == "-r" ) || ( arg == "resolution" ) ) {
//         if ( currArg+1 >= argc ) {
//           printUsage();
//           exit ( 0 );
//         } else {
//           boost::regex resreg;
//           resreg.assign ( "([0-9]+)x([0-9]+)" );
//           boost::cmatch matches;
//           if ( boost::regex_match ( argv[++currArg], matches, resreg ) ) {
//             std::string widthstr ( matches[1].first, matches[1].second );
//             std::string heightstr ( matches[2].first, matches[2].second );
//             width = atoi ( widthstr.c_str() );
//             height = atoi ( heightstr.c_str() );
//           } else {
//             std::cout << "Give resolution in form <height>x<widht> , i.e. 800x600\n --help for more information" << std::endl;
//           }
//         }
//       }
  return true;
}

void Scene::addLight ( const Light& l ) {
  lights.push_back ( l );
}

const PhongMaterial& Scene::getMaterial ( const std::string& name )  {
  if ( material.find ( name ) == material.end() )
    return defaultMaterial;
  else
    return material[name];
}

void Scene::updatePhotonMap() {
  RadianceRay path ( *this );

  unsigned int hitc = 0;
  float delta = ( 2.0 * M_PI ) / SPHERE_SAMPLE;
  float delta05 = delta * 0.5;
  float delta025 = delta05 * 0.5;
  const Vector3D power ( lights[0].getColor().getRGB() );
  float rand1 = 0.0, rand2 = 0.0;
  for ( float angle1 = 0.0f; angle1 < 2.0f *  M_PI; angle1+= delta )
    for ( float angle2 = 0.0f; angle2 < 2.0f * M_PI; angle2+= delta ) {
      rand1 = ( rand() / ( RAND_MAX + 1.0 ) ) * delta05 - delta025;
      rand2 = ( rand() / ( RAND_MAX + 1.0 ) ) * delta05 - delta025;
      //       float angle1 = (rand() / (RAND_MAX + 1.0))* 2.0 * M_PI;
      //       float angle2 = (rand() / (RAND_MAX + 1.0))* 2.0 * M_PI;
      //       std::cout << "a1:" << angle1 << "  a2:" << angle2 << "\n";
      /*      return Vector3D(  cos(angle1)*sin(angle2),
                              cos(angle2),
                              sin(angle1)*sin(angle2));*/
      Vector3D dir ( cos ( angle1 + rand1 )  * sin ( angle2 + rand2 ), cos ( angle2 + rand2 )  , sin ( angle1 + rand1 ) * sin ( angle2 + rand2 ) );
      //       Vector3D dir( cos(angle1) , 0.0f , sin(angle1));

      path.setDirection ( dir );
      path.setStart ( lights[0].getPosition() );
      path.setMin ( 0.0f );
      path.setMax ( UNENDLICH );
      path.getClosestIntersection().reset();
      recievePhoton ( path, power, 1 );
    }

  delta *= 2.0f;
  unsigned int cpcount = 0;
  for ( float angle1 = 0.0f; angle1 < 2.0f *  M_PI; angle1+= delta )
    for ( float angle2 = 0.0f; angle2 < 2.0f * M_PI; angle2+= delta ) {
      ++cpcount;
      rand1 = ( rand() / ( RAND_MAX + 1.0 ) ) * delta05 - delta025;
      rand2 = ( rand() / ( RAND_MAX + 1.0 ) ) * delta05 - delta025;
      Vector3D dir ( cos ( angle1 + rand1 )  * sin ( angle2 + rand2 ), cos ( angle2 + rand2 )  , sin ( angle1 + rand1 ) * sin ( angle2 + rand2 ) );

      path.setDirection ( dir );
      path.setStart ( lights[0].getPosition() );
      path.setMin ( 0.0f );
      path.setMax ( UNENDLICH );
      path.getClosestIntersection().reset();
      const Intersection &ir = geometry->getClosestIntersection ( path );
      if ( path.didHitSomething() && ir.triangle->isSpecular() ) {
        recieveCausticPhoton ( path, power, 3 );
        ++hitc;
      }
    }

  std::cout << "hitc " << hitc << std::endl;

  photonMap->scale_photon_power ( 3.0f/PHOTON_COUNT );
  photonMap->balance();
  causticsMap.scale_photon_power ( 10.0f/cpcount );
  causticsMap.balance();
}

void Scene::recievePhoton ( RadianceRay& photon, const Vector3D& power, unsigned int depth ) {
  // find intersection of photon with geometry

  const Intersection &ir = geometry->getClosestIntersection ( photon );
  if ( !photon.didHitSomething() )
    return;
  const Triangle &triangle = *ir.triangle;
  PhotonBehavior behav = ABSORB;
  if ( depth == 0 )
    if ( triangle.isDiffuse() )
      behav = ABSORB;// in case we have to abort recursion and material is somehow diffuse
    else
      return;
  else
    behav = triangle.getMaterial().spinRoulette();
  const Vector3D normal ( triangle.getNormalAt ( ir ) );
  switch ( behav ) {
    case ( DIF_REFLECT ) : {  // for lambertian surfaces
      const Vector3D localV ( normal % triangle.getU() );
      float angle1 = ( rand() / ( RAND_MAX + 1.0f ) ) * M_PI;
      float angle2 = ( rand() / ( RAND_MAX + 1.0f ) ) * 2.0f * M_PI;
      const Vector3D newDirection ( ( sin ( angle1 ) * normal + cos ( angle1 ) * triangle.getU() ) * sin ( angle2 ) +
                                    cos ( angle2 ) * localV );
      RadianceRay reflected ( ir.intersectionPoint, newDirection, *this );
      reflected.setIgnore ( ir.triangle );
      recievePhoton ( reflected, power, depth-1 );
      // no break here, because photon gets stored every time it hits a diffuse surface
    }
    case ( ABSORB ) : {
      float avg = ( triangle.getMaterial().diffuse[0]+triangle.getMaterial().diffuse[1]+triangle.getMaterial().diffuse[2] ) *.33333;
      Vector3D scaledPower ( power * ( 1.0f / avg ) );
      photonMap->store ( scaledPower.value, ir.intersectionPoint.value, photon.getDirection().value );
      break;
    }
    case ( TRANSMIT ) : {
      RadianceRay reflectedRay ( ir.intersectionPoint, photon.getDirection(), *this );
      reflectedRay.setIgnore ( ir.triangle );
      const Intersection &inters = getGeometry().getClosestIntersection ( reflectedRay );
      if ( inters.triangle != 0 ) {
        Vector3D refrDirOut = Ray::refractRay ( photon.getDirection(), inters.triangle->getNormalAt ( inters ) * -1.0, 1.3, 1.0 );
        RadianceRay r2 = RadianceRay ( inters.intersectionPoint, refrDirOut, *this );
        r2.setIgnore ( ir.triangle );
        this->recievePhoton ( r2, power, depth-1 );
      }
      break;
    }
    case ( SPEC_REFLECT ) : {
      RadianceRay reflected ( ir.intersectionPoint, photon.getDirection().reflect ( normal ), *this );
      this->recievePhoton ( reflected, power, depth-1 );
      break;
    }
  }
}

void Scene::recieveCausticPhoton ( RadianceRay& photon, const Vector3D& power, unsigned int depth ) {
  // find intersection of photon with geometry

  const Intersection &ir = geometry->getClosestIntersection ( photon );
  if ( !photon.didHitSomething() )
    return;
  const Triangle &triangle = *ir.triangle;
  PhotonBehavior behav = ABSORB;
  if ( depth == 0 )
//       if ( ( triangle.getMaterial().alpha > 0.0 ) && ( triangle.getMaterial().reflection < 1.0 ) )
//         behav = ABSORB;// in case we have to abort recursion and material is somehow diffuse
//       else
    return;
  else
    behav = triangle.getMaterial().spinRoulette();
  const Vector3D normal ( triangle.getNormalAt ( ir ) );
  switch ( behav ) {
    case ( DIF_REFLECT ) :
          case ( ABSORB ) : {
        float avg = ( triangle.getMaterial().diffuse[0]+triangle.getMaterial().diffuse[1]+triangle.getMaterial().diffuse[2] ) *.33333;
        Vector3D scaledPower ( power * ( 1.0f / avg ) );
        causticsMap.store ( scaledPower.value, ir.intersectionPoint.value, photon.getDirection().value );
        break;
      }
    case ( TRANSMIT ) : {
      RadianceRay reflectedRay ( ir.intersectionPoint, photon.getDirection(), *this );
      reflectedRay.setIgnore ( ir.triangle );
      const Intersection &inters = getGeometry().getClosestIntersection ( reflectedRay );
      if ( inters.triangle != 0 ) {
        Vector3D refrDirOut = Ray::refractRay ( photon.getDirection(), inters.triangle->getNormalAt ( inters ) * -1.0, 1.3, 1.0 );
        RadianceRay r2 = RadianceRay ( inters.intersectionPoint, refrDirOut, *this );
        r2.setIgnore ( ir.triangle );
        this->recieveCausticPhoton ( r2, power, depth-1 );
      }
      break;
    }
    case ( SPEC_REFLECT ) : {
      RadianceRay reflected ( ir.intersectionPoint, photon.getDirection().reflect ( normal ), *this );
      this->recieveCausticPhoton ( reflected, power, depth-1 );
      break;
    }
  }
}

Scene::~Scene() {
}
