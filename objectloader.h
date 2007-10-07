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

/**
Util class to load .obj files

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/

class AccelerationStruct;

class ObjectLoader{
public:
    ObjectLoader();
    static bool loadMonkey(char* filename, AccelerationStruct *tl);
    ~ObjectLoader();

};

#endif
