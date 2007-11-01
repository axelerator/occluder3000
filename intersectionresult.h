//
// C++ Interface: intersectionresult
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INTERSECTIONRESULT_H
#define INTERSECTIONRESULT_H

#include "vector3d.h"

/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class IntersectionResult {
public:
    IntersectionResult();
    IntersectionResult(const IntersectionResult& ir);
    IntersectionResult& operator=(const IntersectionResult& v);
    
    ~IntersectionResult();
    
    const Vector3D& calcPOI() ;
    const Vector3D& getPOI() const { return poi;}
    Vector3D e1, e2;
    fliess u,v,t;
    Vector3D orig;
    unsigned int triangleIdx;
    
    private:
      Vector3D poi;
};

#endif
