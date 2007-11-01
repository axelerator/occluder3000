//
// C++ Implementation: regulargrid
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "regulargrid.h"
#include "triaabb.h"
#include "GL/gl.h"
RegularGrid::RegularGrid(const Scene& scene, fliess cellsize):
AccelerationStruct(scene), grid(0), cellsize(cellsize), cellsizeInvert(1.0/cellsize) {}


RegularGrid::~RegularGrid() {
    const unsigned int gridsize = cellCount[0]*cellCount[1]*cellCount[2];
    if ( grid ) {
        for ( fliess xpos = bounds[0]; xpos < bounds[1] ; xpos += cellsize )
            for ( fliess ypos = bounds[2]; ypos < bounds[3] ; ypos += cellsize )
                for ( fliess zpos = bounds[4]; zpos < bounds[5] ; zpos += cellsize ) {
                    const unsigned int cellIdx = (int)((zpos - bounds[4]) / cellsize) * slabSize // slabs in z-direction
                                                 + (int)((ypos - bounds[2]) / cellsize) * cellCount[0] // + y-count rows
                                                 + (int)((xpos - bounds[0]) / cellsize);
                    if (cellIdx > gridsize ) {
                        std::cerr << "IndexOutOfBounds: Tried to access non existent RegularGrid cell\n";
                    }
                    if ( grid[cellIdx] != 0 )
                        delete grid[cellIdx];
                }
        delete grid;
    }
}

int foo = 0;
const RGBvalue RegularGrid::trace ( Ray& r, unsigned int depth ) {
// Fast Voxel Traversal by Amanatides and Woo

// :: Initialization phase ::
//    - Identify voxel in which ray origins
    Vector3D position(r.getStart());
    unsigned int currentVox[3];
    const Vector3D& rs = r.getStart();
    if   (( rs[0] < bounds[0] ) || ( rs[1] < bounds[2] ) || ( rs[2] < bounds[4] )
            || ( rs[0] > bounds[1] ) || ( rs[1] > bounds[3] ) || ( rs[2] > bounds[5] )) {
        // ray origin lies outside grid => calc first intersecting voxel
        BoxRayIntersect bri;
        gridBox.intersect(r, 0.0f, 1000.0, bri);
        if (bri.hit) {
            currentVox[0] = (int)((bri.poi[0] - bounds[0]) * cellsizeInvert) ;
            currentVox[1] = (int)((bri.poi[1] - bounds[2]) * cellsizeInvert) ;
            currentVox[2] = (int)((bri.poi[2] - bounds[4]) * cellsizeInvert) ;
            position = bri.poi;
        } else {
            return RGBvalue(0.0, 0.0, 0.0);
        }
    } else {
        //std::cout << currentVoxCandidate[0] <<  "  |  " << currentVoxCandidate[1] <<  "  |  " << currentVoxCandidate[2] <<  "\n";
        currentVox[0] = (int)((r.getStart()[0] - bounds[0]) * cellsizeInvert) ;
        currentVox[1] = (int)((r.getStart()[1] - bounds[2]) * cellsizeInvert) ;
        currentVox[2] = (int)((r.getStart()[2] - bounds[4]) * cellsizeInvert) ;
    }

    int step[3]; // step to next voxel in direction of ray ( -1 or 1 )
    step[0] = ( r.getDirection()[0] > 0 ) ? 1 : -1;
    step[1] = ( r.getDirection()[1] > 0 ) ? 1 : -1;
    step[2] = ( r.getDirection()[2] > 0 ) ? 1 : -1;
// :: Incremental phase ::
    unsigned int gridIdx = currentVox[2] * slabSize // slabs in z-direction
                           + currentVox[1] * cellCount[0] // + y-count rows
                           + currentVox[0];
    IntersectionResult ir; // will store hit info for triangles in current voxel and ray
    fliess zBuffer = UNENDLICH;
    const Triangle *hitTriangle = 0;
    IntersectionResult closestRi;
    fliess tMax[3]; // these values indicate how much we can travel through the current Voxel
    //  into direction of one component until we hit the next voxel in that direction.
    // (in units of the direction vector of r)
    Ray voxelRay(position, r.getDirection());
    while ( !hitTriangle && (currentVox[0] < cellCount[0]) 
          && (currentVox[1] < cellCount[1])  && (currentVox[2] < cellCount[2])  ) {
        voxelRay.setStart(position);
        gridIdx =     currentVox[2] * slabSize // slabs in z-direction
                    + currentVox[1] * cellCount[0] // + y-count rows
                    + currentVox[0];
        zBuffer = 10000.0;
        hitTriangle = 0;


        for (unsigned char i = 0; i < 3; ++i )
            tMax[i] = ((bounds[2*i] + (currentVox[i] + (step[i] > 0)) * cellsize) - position[i] ) / r.getDirection()[i];

        fliess smallestT = tMax[0];// determine the closest voxel and axis on which it will follow
        unsigned char component = 0;
        if ( tMax[1] < smallestT) {
            smallestT = tMax[1];
            component = 1;
        }
        if ( tMax[2] < smallestT) {
            smallestT = tMax[2];
            component = 2;
        }

        if ( grid[gridIdx] ) {
         std::vector<int>::const_iterator iter;
          for (iter =  grid[gridIdx]->begin() ; iter != grid[gridIdx]->end() ; ++iter ) {
          const Triangle& currTri = triangles[*iter];
              if ( currTri.intersect(voxelRay, ir ) ) {
                  fliess currentZ = ( ir.calcPOI() - r.getStart() ).lengthSquare();
                  if ( currentZ < zBuffer ) {
                      zBuffer = currentZ;
                      closestRi = ir;
                      hitTriangle = &(currTri);
                  }
              }
          }
         }

        if ( !hitTriangle ) {
            // proceed to next voxel
             currentVox[component] += step[component];
            position += r.getDirection() * smallestT;
        }
    } // end of incremental phase of voxel traversal

    if ( hitTriangle != 0 )  {

        Vector3D n ( hitTriangle->getNormalAt ( closestRi ) );
        RGBvalue lightVessel ( 0.0, 0.0, 0.0 );
        const PhongMaterial& mat = hitTriangle->getMaterial();
        const std::vector<Light> lights = scene.getLights();
        std::vector<Light>::const_iterator it;
        for ( it = lights.begin(); it!=lights.end(); ++it ) {
            const Light& light = *it;

            Vector3D l ( light.getPosition() - closestRi.getPOI() );
            l.normalize();
            fliess dif = n * l;
            //dif = 1.0;
            if (dif > 0.0)
                lightVessel.add ( dif * mat.diffuse[0] * light.getColor().getRGB()[0],
                                  dif * mat.diffuse[1] * light.getColor().getRGB()[1],
                                  dif * mat.diffuse[2] * light.getColor().getRGB()[2] );
        }
        return lightVessel;
    } else
        return  RGBvalue ( 0, 0, 0 );

}

/**
  * Assisgns the triangles to the according gridcells
  * Setbounds has to be called first !!
  **/
void RegularGrid::construct() {
    if ( !boundsSet ) {
        std::cerr << "RegularGrid is tried to be constructed without setting bounds first !\n" ;
        return;
    }
    cellCount[0] = (int)Fliess::ceil((bounds[1] - bounds[0]) / cellsize);
    cellCount[1] = (int)Fliess::ceil((bounds[3] - bounds[2]) / cellsize);
    cellCount[2] = (int)Fliess::ceil((bounds[5] - bounds[4]) / cellsize);
    gridBox.parameters[0] = Vector3D(bounds[0],bounds[2],bounds[4]);
    gridBox.parameters[1] = Vector3D(bounds[1],bounds[3],bounds[5]);
    slabSize = cellCount[0] * cellCount[1];
    std::cout << "cells in x: " << cellCount[0] << " cells in y " << cellCount[1] << "  cells in z: " << cellCount[2] <<  "\n";

    // allocate a pointer to a vector of triangle indices for each cellsize
    const unsigned int gridsize = cellCount[0]*cellCount[1]*cellCount[2];
    grid = new std::vector<int>*[gridsize];
    memset(grid, 0, gridsize * sizeof(std::vector<int>*));
    std::vector<Triangle>::const_iterator iter;
    unsigned int component_min;
    unsigned int trianglesIdx = 0;
    const Vector3D boxhalfSize(cellsize*0.5);
    const fliess cellsizeHalf = cellsize * 0.5;
    for ( iter = triangles.begin(); iter != triangles.end() ; ++iter ) {
        const Triangle& currTri = *iter;
        // calc bb of triangle, to determine region of cells that can intersect
        fliess currentBounds[] = { UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH };
        for (unsigned int i = 0; i < 3; ++i) {
            const Vector3D& v = currTri.getPoint(i);
            for ( unsigned char component = 0; component < 3 ; ++component ) {
                component_min = 2 * component;
                if (v[component] < currentBounds[component_min])
                    currentBounds[component_min] = v[component];
                if (v[component] > currentBounds[component_min+1])
                    currentBounds[component_min+1] = v[component];
            }
        }
        // trim boundings to gridcells
        unsigned int intbounds[6];
        //std::cout << "curentbnds: " << currentBounds[0] << " - " << currentBounds[1] << "   "<< currentBounds[2] << " - " << currentBounds[3] << "   "<< currentBounds[4] << " - " << currentBounds[5] << "\n";


        intbounds[0] = (int)Fliess::floor((currentBounds[0] - bounds[0]) / cellsize) ;
        intbounds[1] = (int)Fliess::floor( (currentBounds[1] - bounds[0]) / cellsize) + 1;
        intbounds[2] = (int)Fliess::floor((currentBounds[2] - bounds[2]) / cellsize) ;
        intbounds[3] = (int)Fliess::floor( (currentBounds[3] - bounds[2]) / cellsize) + 1 ;
        intbounds[4] = (int)Fliess::floor((currentBounds[4] - bounds[4]) / cellsize) ;
        intbounds[5] = (int)Fliess::floor( (currentBounds[5] - bounds[4]) / cellsize) + 1 ;

        //std::cout << "curentbnds: " << intbounds[0] << " - " << intbounds[1] << "   "<< intbounds[2] << " - " << intbounds[3] << "   "<< intbounds[4] << " - " << intbounds[5] << "\n";
        for ( unsigned int xpos = intbounds[0]; xpos < intbounds[1] ; ++xpos )
            for ( unsigned int ypos = intbounds[2]; ypos < intbounds[3] ; ++ypos)
                for ( unsigned int zpos = intbounds[4]; zpos < intbounds[5] ; ++zpos  ) {
                    if (TriAABBIntersect::intersects(Vector3D(bounds[0] + xpos * cellsize + cellsizeHalf, bounds[2] + ypos * cellsize + cellsizeHalf, bounds[4] + zpos * cellsize + cellsizeHalf), boxhalfSize, currTri)) {

                        const unsigned int cellIdx =   zpos * slabSize // slabs in z-direction
                                                       + ypos * cellCount[0] // + y-count rows
                                                       + xpos;
                        if (cellIdx > gridsize ) {
                            std::cerr << "IndexOutOfBounds: Tried to access non existent RegularGrid cell\n";
                        }
                        if ( grid[cellIdx] == 0) {
                            grid[cellIdx] = new std::vector<int>;
                        }

                        grid[cellIdx]->push_back(trianglesIdx);
                    }
                }

        ++trianglesIdx;
    }
//         std::cout << "gridsize " << gridsize << ":\n";
//     for( unsigned int i = 0; i < gridsize ; ++i){
//       if ( grid[i] != 0 )
//       std::cout << "Cell " << i << " contains " << grid[i]->size() << " triangles\n";
//     }
}
/*
void unitQuad() {
 glBegin(GL_QUADS);
    glVertex3f(-1.0,  1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f( 1.0, -1.0, 0.0);
    glVertex3f( 1.0,  1.0, 0.0);
  glEnd();
}
void drawMinicube(fliess x,fliess y,fliess z, fliess size) {
        glPushMatrix();
        glTranslatef(x,y,z);
        glScalef(size*0.5, size*0.5, size*0.5);
        glPushMatrix();
          glTranslatef(0.0, 0.0, -1.0);
          unitQuad();
        glPopMatrix();
        glPushMatrix();
          glTranslatef(0.0, 0.0, 1.0);
          unitQuad();
        glPopMatrix();
        glPushMatrix();
          glTranslatef(0.0, 1.0, 0.0);
          glRotatef(90.0, 1.0, 0.0, 0.0);
          unitQuad();
        glPopMatrix();

        glPushMatrix();
          glTranslatef(0.0, -1.0, 0.0);
          glRotatef(90.0, 1.0, 0.0, 0.0);
          unitQuad();
        glPopMatrix();
        glPopMatrix();
}

void RegularGrid::drawGL() {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
       glDisable(GL_LIGHTING);
       glDisable(GL_DEPTH_TEST);
       glEnable(GL_BLEND);
       fliess cellsizeHalf = cellsize * 0.5;
       for (  int xpos = 0; xpos < cellCount[0] ; ++xpos )
            for (  int ypos = 0; ypos < cellCount[1] ; ++ypos)
                for (  int zpos = 0; zpos < cellCount[2] ; ++zpos ) {
                  const unsigned int cellIdx = zpos * slabSize // slabs in z-direction
                                             + ypos * cellCount[0] // + y-count rows
                                             + xpos;
                  if ( grid[cellIdx]) {
                     glColor4f(0.2, 0.2, 0.7, 1.0);
                  } else
                    glColor4f(0.2, 0.2, 0.2,0.5);
                  drawMinicube(bounds[0] + xpos * cellsize + cellsizeHalf, bounds[2] + ypos * cellsize + cellsizeHalf, bounds[4] + zpos * cellsize + cellsizeHalf, cellsize);
                }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_LIGHTING);
        AccelerationStruct::drawGL();
}
*/
