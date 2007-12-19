//
// C++ Interface: bih2
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BIH2_H
#define BIH2_H
#include "accelerationstruct.h"
#include "simplevector.h"

/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BIH : public AccelerationStruct {
  public:
    BIH ( const Scene& scene );

    ~BIH();

    virtual const RGBvalue trace ( RadianceRay& r, unsigned int depth );
    virtual bool isBlocked(Ray& r);
    virtual void construct();
    virtual const Intersection& getClosestIntersection(RadianceRay& r);


    bool isConsistent();
  private:
    struct BihNode {

      unsigned char type;
      struct BihNode *leftchild;

      union {
        float planes[2];
        unsigned int leafContent[2];
      };
    };
    typedef struct BihNode BihNode;

    struct Stack {
      const BihNode *node;
      float tmin,tmax;
    };

    typedef Stack Stack;

    void subdivide ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth );
    void traverse ( const BihNode& node, RadianceRay& r, float tmin, float tmax, unsigned int depth );
    void traverseIterative ( RadianceRay& r );
    bool traverseShadow ( Ray& r );
    bool checkConsistency ( BihNode *node );

    unsigned int *triangleIndices;
    unsigned int minimalPrimitiveCount;
    unsigned int maxDepth; // maximal Depth of recursion of subdivision


    SimpleVector<BihNode> nodes;
    int markednode;
};


#endif
