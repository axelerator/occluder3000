//
// C++ Interface: boundaryintervalhierarchy
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BOUNDARYINTERVALHIERARCHY_H
#define BOUNDARYINTERVALHIERARCHY_H

#include <accelerationstruct.h>
#include "GL/gl.h"

class BIHTreeNode {
public:
    virtual ~BIHTreeNode(){};
    virtual void drawGL() = 0;
    virtual bool traverse(Ray& r, IntersectionResult& ir, fliess tmin, fliess tmax ) = 0;
};




/**
  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BoundaryIntervalHierarchy : public AccelerationStruct
{
  public:
    BoundaryIntervalHierarchy(const Scene& scene);

    ~BoundaryIntervalHierarchy();

    virtual const RGBvalue trace ( Ray& r, unsigned int depth );
    virtual void construct();
    static void drawBB(fliess *bounds);
    
    
    
    private:
    BIHTreeNode *subdivide(fliess *currBounds, std::vector<int> &objects, unsigned int depth  = 0);
    virtual void drawGL();
    bool testObjctsVsBB(fliess *bounds, std::vector<int> &objects);
/*
    typedef struct {
      int index; // TODO: use lowestbits:axis(00,01,10)orleaf(11)
      unsigned char axis;
      union {
        int items;//leafonly
        float clip[2];//internalnodeonly
      };
    } BIH_Node ;*/
    BIHTreeNode *root;

};


class BIHInternalNode : public BIHTreeNode {
  public:
  BIHInternalNode(unsigned char axis, fliess leftMax, fliess rightMin, fliess *bb):
  axis(axis){
    clip[0] = leftMax;
    clip[1] = rightMin;
    memcpy(this->bb, bb, sizeof(fliess) * 6);
  }
  
  virtual void drawGL() {
    BoundaryIntervalHierarchy::drawBB(bb);
    children[0]->drawGL();
    children[1]->drawGL();
  }
  
  virtual bool traverse(Ray& r, IntersectionResult& ir, fliess tmin, fliess tmax ) ;
  
  BIHTreeNode *children[2];
  unsigned char axis;
  fliess clip[2];
  fliess bb[6];
};


class BIHLeaf : public BIHTreeNode {
  public: 
    BIHLeaf(int idx):index(idx){}
    const unsigned int index;
      virtual void drawGL() {
        glColor4f(1.0, 1.0, 1.0, 0.3);
        glBegin(GL_TRIANGLES);
//          b->getTriangle(index).drawGL();
        glEnd();
      }
      
  virtual bool traverse(Ray& r, IntersectionResult& ir, fliess tmin, fliess tmax );
  static BoundaryIntervalHierarchy *b;
};

#endif
