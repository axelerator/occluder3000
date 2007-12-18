//
// C++ Interface: phongmaterial
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H

/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class PhongMaterial{
public:
    PhongMaterial();

    ~PhongMaterial();
    float diffuse[3];
    float alpha;
    float refract;
    float reflection;
};

#endif
