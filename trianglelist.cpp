//
// C++ Implementation: trianglelist
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "trianglelist.h"
#include "light.h"
#include "debug.h"

Trianglelist::Trianglelist ( const Scene& scene ) :
AccelerationStruct(scene) {}

Trianglelist::~Trianglelist() {}



const RGBvalue Trianglelist::trace ( Ray& r, unsigned int depth ) {
    IntersectionResult ir;

    fliess zvalue = 10000.0;
    fliess currentZ;
    Triangle* closestTriangle = 0;
    Vector3D poi;
    Vector3D finalPoi;
    for ( std::vector<Triangle>::iterator it = triangles.begin(); it!=triangles.end(); ++it ) {
        if ( ( *it ).intersect( r,ir )) {
            poi = ir.calcPOI();
            currentZ = ( poi - r.getStart() ).lengthSquare();
            if (currentZ > 0 && currentZ < zvalue ) {
                zvalue = currentZ;
                closestTriangle = & ( *it );
                finalPoi = poi;
            }
        }
    }
    if ( closestTriangle != 0 )  {
       
        Vector3D n ( closestTriangle->getNormalAt ( ir ) );
        RGBvalue lightVessel ( 0.0, 0.0, 0.0 );
        const PhongMaterial& mat = closestTriangle->getMaterial();
        const std::vector<Light> lights = scene.getLights();
        std::vector<Light>::const_iterator it;
        for ( it = lights.begin(); it!=lights.end(); ++it ) {
            const Light& light = *it;
//            Vector3D l ( finalPoi - light.getPosition() );
            Vector3D l ( light.getPosition() - finalPoi );
            l.normalize();
            fliess dif = n * l;
            //dif = 1.0;
            if (dif > 0.0)
              lightVessel.add ( dif * mat.diffuse[0] * light.getColor().getRGB()[0], 
                                dif * mat.diffuse[1] * light.getColor().getRGB()[1], 
                                dif * mat.diffuse[2] * light.getColor().getRGB()[2] );
/*              lightVessel.add ( dif , 
                                dif, 
                                dif );   */                             
        }
        return lightVessel;
    } else
        return  RGBvalue ( 0, 0, 0 );
}


void Trianglelist::construct() {
}
