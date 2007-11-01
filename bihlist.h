//
// C++ Interface: bihlist
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BIHLIST_H
#define BIHLIST_H

#include <accelerationstruct.h>


/**
Implements the BIH as List

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/


class BihList : public AccelerationStruct {
  public:
    BihList ( const Scene& scene );

    ~BihList();

    virtual const RGBvalue trace ( Ray& r, unsigned int depth );
    virtual void construct();

  private:
    struct BihNode {
      union {
        struct BihNode *nextLeft; // contains nodetype in lower 2 bits 00,01,10 = axis of inner nodetype
        int axis ;        //                                    11 = leaf
      };
      union {
        float planes[2];
        unsigned int leafContent[2];
      };
    };
    typedef struct BihNode BihNode;
    void subdivide ( BihNode *thisNode, unsigned int start, unsigned int end, const fliess *currBounds, unsigned int depth );

    
    unsigned int *triangleIndices; // array of triangleIndices on which the in place sorting will happen
    unsigned int maxDepth; // maximal Depth of recursion of subdivision
    unsigned int minimalPrimitiveCount; // this amount of primitives is low enough for us to create a leaf
    std::vector<BihNode> bihNodes;

  class Stacknode {
    public:
    Stacknode(fliess n, fliess f, BihNode *nd, Stacknode *p) : tmin(n), tmax(f), node(nd), prev(p) {}
    fliess tmin;
    fliess tmax;
    BihNode *node;
    Stacknode *prev;
  };

};

#endif
