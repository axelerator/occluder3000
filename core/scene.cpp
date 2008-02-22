//
// C++ Implementation: scene
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scene.h"
#include <fstream>
#include <iostream>
#include <boost/regex.hpp>
#include "stringutil.h"
#include "accelerationstructure.h"
#include "primitivelist.h"
#include "objectloader.h"
#include "flatshader.h"
#include "specularshader.h"
#include "directshader.h"
#include "light.h"
#include "intersectionsse.h"
#include "regulargrid.h"
#include "bih.h"
#include "bihcompact.h"

using namespace Occluder;

Scene::Scene():
geometry(new PrimitiveList(*this)), defaultShader( "default", *this ) {}


Scene::~Scene() {
    std::vector<Shader*>::iterator shaderIter;
    for ( shaderIter = shader.begin(); shaderIter != shader.end(); ++ shaderIter )
        delete *shaderIter;
    shader.clear();

    std::vector<Light*>::iterator lightIter;
    for ( lightIter = lights.begin(); lightIter != lights.end(); ++ lightIter )
        delete *lightIter;
    lights.clear();
}

const bool Scene::loadFromFile ( const std::string& filename ) {
    std::vector<std::string> file;
    std::string line;
    file.clear();
    std::ifstream infile ( filename.c_str(), std::ios_base::in );
    boost::regex reCfgLine;
    boost::regex reSctLine;
    std::string reStrConfigLine = "(\\w+)\\s*=\\s*(.*)(#.*)?";
    std::string reStrSectionLine = "\\[(\\w+)(:\\w+)?(:\\w+)?\\](#.*)?";

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
    std::string geometryfile = "";

    while ( ( it != file.end() && ( ( *it ) [0]  != '[' ) ) ) {
        const std::string& line = ( *it ) ;
        if ( boost::regex_match ( line.c_str(), matches, reCfgLine ) ) {
            const std::string key ( matches[1].first, matches[1].second );
            const std::string value ( matches[2].first, matches[2].second );
            if ( key == "acceleration" ) {
                if ( geometry )
                    delete geometry;
        if ( value == "grid" )
          geometry = new RegularGrid (2, *this );
        else if ( value == "bih" )
          geometry = new Bih ( *this, 1 );
        else if ( value == "bihcompact" )
          geometry = new BihCompact ( *this, 1 );
//         else if ( value == "kd" )
//           geometry = new KdTree ( *this );
        else {
                std::cout << "Accelerationstruct " << value << " not available. Falling back to default(list). Choose { grid, bih, kd }" << std::endl;
                geometry = new PrimitiveList ( *this );
        }
            } else if ( key == "geometry" ) {
                if ( filename.find ( "/" ) != std::string::npos )
                    geometryfile = filename.substr ( 0, filename.find_last_of ( "/" ) +1 )  + value;
                else
                    geometryfile = value;
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
    std::string lastMat("default");
    do {
        const std::string& line = ( *it ) ;
        if ( boost::regex_match ( line.c_str(), matches, reSctLine ) ) {
            const std::string value ( matches[1].first, matches[1].second );
            if ( value == "camera" )
                currentObject = CAMERA;
            else if ( value == "light" ) {
                currentObject = LIGHT;
                lights.push_back(new Light());
            } else if ( value == "material" ) {
                currentObject = MATERIAL;
                if ( matches.size() < 3 )
                    std::cerr << "Material in line" << linenr << "is missing identifier and/or type" << std::endl;
                else {
                    const std::string type = std::string( matches[2].first, matches[2].second ).substr(1);
                    Shader *newshader = 0;
                    const std::string shadername = std::string( matches[3].first, matches[3].second ).substr(1);
                    if ( type == "constant" )
                        newshader = new FlatShader(shadername, *this);
                    else if ( type == "direct" )
                        newshader = new DirectShader(shadername, *this);
                    else if ( type == "specular" )
                        newshader = new SpecularShader(shadername, *this);
                    else
                        std::cerr << "Materialtype " << type << " in line" << linenr << "is not valid" << std::endl;
                    if ( newshader ) {
                        lastMat = shadername;
                        shader.push_back(newshader);
                        shaderMap.insert(std::pair<std::string, const Shader*>(shadername, newshader));
                    }
                }
            } else
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
                case CAMERA:
                    cam.setPropertyFromString ( key, value );
                    break;
                case LIGHT:
                    (lights.back())->setPropertyFromString(key, value);
                    break;
                case MATERIAL:
                    shader.back()->setPropertyFromString(key, value);
                    break;
                default:
                    std::cerr << "No object that " << line << " coud be assigned to, in line " << linenr << std::endl;
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

    if ( geometryfile != "")
        if ( geometryfile.find ( ".obj" ) != std::string::npos )
            ObjectLoader::loadOBJ ( geometryfile, *this );
        else if ( geometryfile.find ( ".ra2" ) != std::string::npos )
            ObjectLoader::loadRA2 ( geometryfile, *this );
        else
            std::cerr << "Unable to recognize file extension of geometry file: " << geometryfile << std::endl;

    return true;
}


size_t Scene::addVertex(const Vec3& v) {
    vertices.push_back(v);
    aabb.update(v);
    return vertices.size() - 1;
}

size_t Scene::getVertexCount() {
    return vertices.size();
}


size_t Scene::addPrimitive(const Primitive& prim) {
    primitives.push_back(prim);
    return primitives.size() - 1;
}

const Intersection Scene::trace(const RaySegment& ray) const {
    return geometry->getFirstIntersection(ray);
}

const IntersectionSSE Scene::trace(const RaySegmentSSE& rays) const {
  IntersectionSSE result;
  geometry->determineFirstIntersection(rays, result);
  if ( !result.isEmpty() )
    result.resolvePrimitiveIds(*this, rays);
  return result;
}


const Shader& Scene::getShader(const std::string& name) const {
  std::map<std::string, const Shader*>::const_iterator mat = shaderMap.find(name);
  if ( mat == shaderMap.end() ) {
    return defaultShader;
  } else
    return *((*mat).second);
}
