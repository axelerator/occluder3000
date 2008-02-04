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
#include "sse4.h"
class RayPacket;
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BIH : public AccelerationStruct {
  public:
    BIH ( const Scene& scene );

    ~BIH();

    virtual const RGBvalue trace ( RadianceRay& r, unsigned int depth ) const;
    virtual bool trace ( RayPacket& rp, unsigned int depth = 5 )const;
    virtual bool isBlocked(Ray& r) const;
    virtual void construct();
    virtual const Intersection& getClosestIntersection(RadianceRay& r) const;
    virtual void analyze() const;
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

    struct MultiStack {
      const BihNode *node;
      __m128 tmin,tmax;
    };

    typedef MultiStack MultiStack;

    void subdivide ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth );
    void subdivide2 ( BihNode &thisNode, unsigned int start, unsigned int end, const float *currBounds, unsigned int depth );
    void traverseIterative ( RadianceRay& r ) const;
    void traverseRecursive ( RadianceRay& r, const BihNode & node, float tmin, float tmax) const;
    bool traverseShadow ( Ray& r ) const;
    void recurse(RayPacket& rp) const;
    
    bool checkConsistency ( BihNode *node );
    static unsigned int depth(BihNode &node);
    static unsigned int leafcount(BihNode &node);
    unsigned int *triangleIndices;
    unsigned int minimalPrimitiveCount;
    unsigned int maxDepth; // maximal Depth of recursion of subdivision


    SimpleVector<BihNode> nodes;
    int markednode;
};


#endif
