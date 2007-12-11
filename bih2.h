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


/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BIH2 : public AccelerationStruct {
  public:
    BIH2 ( const Scene& scene );

    ~BIH2();

    virtual const RGBvalue trace ( Ray& r, unsigned int depth );
    virtual void construct();

#ifdef VISUAL_DEBUGGER
    virtual void draw ( GLWidget* context ) const;
    virtual void drawSchema ( GLWidget* context ) const;
    virtual void drawWithNames ( GLWidget* context ) const;
    virtual void select ( int selected );
#endif


    bool isConsistent();
  private:
    struct BihNode {

      unsigned char type;
      struct BihNode *children[2];
#ifdef VISUAL_DEBUGGER
      int idx;
      float bounds[6];
#endif
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
    void traverseIterative ( const BihNode* node, RadianceRay& r, float tmin, float tmax );
    bool traverseShadow ( const BihNode* startnode, Ray& r, float tmin, float tmax, IntersectionResult &ir, const Triangle *ignoreTriangle );
#ifdef VISUAL_DEBUGGER
    const void drawTree ( const BihNode &node, bool drawLeaves , GLWidget *context ) const;
    const Vector3D drawTreeWithNames ( const BihNode &node, int depth, GLWidget* context ) const;
    const Vector3D drawSchemaR ( const BihNode &node, int depth, GLWidget* context ) const;
#endif
    bool checkConsistency ( BihNode *node );


    unsigned int *triangleIndices;
    unsigned int minimalPrimitiveCount;
    unsigned int maxDepth; // maximal Depth of recursion of subdivision


    unsigned int reserved;
    unsigned int occupied;
    BihNode *nodes;
    int markednode;
};


#endif
