//
// C++ Interface: objectloader
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H
#include <string>
namespace Occluder {
class Scene;
/**
Util class to load .obj files

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/

class ObjectLoader{
public:
    ObjectLoader();

    static bool loadOBJ(const std::string& filenam, Scene& scene);
    static bool loadRA2(const std::string& filename, Scene& scene);
    ~ObjectLoader();
private:
  typedef  float ra2_chunk[9];

};
}
#endif
