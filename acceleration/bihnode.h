//
// C++ Interface: BihNodeCompact
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERBihNodeCompact_H
#define OCCLUDERBihNodeCompact_H
#include "bintree.h"

namespace Occluder {

/**
The compact version of a BihNodeCompact

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BihNodeCompact{
public:
      /**
        Makes this node a leaf which contains the primives, referenced by
        by the elements in the primitive index list from element start up to
        element end (including both)
       **/
      void makeLeaf(unsigned int start, unsigned int end);

      /**
        Turns this node into an inner node. 
        @param rightNode pointer to the right child. The left child is
                         the next one to this in memory.
        @param leftMax maximum extent on axis of the left subtree
        @param rightMax minimal extent on axis of the right subtree
        @param axis Axis along which the nod is split
       **/
      void makeInner(BihNodeCompact *rightNode, float leftMax, float rightMin, unsigned char axis);
      bool isLeaf() const;

      /**
        Since the Bih is a binary tree, every inner node has two children
        which can be retrieved via this method.
        @param i which of the two children should be returned (0,1)
        @return the i-th child of this node.(undefined in case of leaf, or i > 1)
       **/
      const BihNodeCompact& getChild(unsigned char i) const;

      unsigned char getAxis() const;

      /**
        only valid on inner node
        @param i for 0 the maximal geometric value of the primtives in the left is returned
                 for 1 the minimal geometric value of the primtives in the right is returned
        @return the extremal value for the i-th child
       **/
      float getExtremum(unsigned char i) const;


      /**
        only valid on leaf node. 
        @return position of the first primitive id which is
        contained in this leaf
       **/
      unsigned int first() const;

      /**
        only valid on leaf node. 
        @return position of the last primitive id which is
        contained in this leaf
       **/
      unsigned int last() const;


      unsigned int leafCount() const;
      unsigned int nodeCount() const;
      AvgInfo avgTrisPerLeaf() const;
      void analyze() const;
      unsigned int treeDepth() const;

private:

union {
  BihNodeCompact* right; // the left child is always stored besides the parent
  long axis;
};

union {
  unsigned int primitives[2];
  float planes[2];
};

} __attribute__ ((aligned (16)));

inline void BihNodeCompact::makeLeaf(unsigned int start, unsigned int end) {
  primitives[0] = start;
  primitives[1] = end;
  axis = 3;
}

inline void BihNodeCompact::makeInner(BihNodeCompact *rightNode, float leftMax, float rightMin, unsigned char axis) {
  planes[0] = leftMax;
  planes[1] = rightMin;
  right = rightNode;
  this->axis |= axis;
}


inline bool BihNodeCompact::isLeaf() const {
  return getAxis() == 3;
}

inline const BihNodeCompact& BihNodeCompact::getChild(unsigned char i) const {
  if ( i == 0 )
    return this[1];
  // for a 4-Byte aligned block of mem the last 2 bits of a pointer
  // are alsways 0. This is exploited in the way that the axis is
  // stored in these 2 bits. To get the actual reference we have to
  // zero out these bits.
  return *((BihNodeCompact*)(axis & 0xFFFFFFFC));
}

inline unsigned char BihNodeCompact::getAxis() const {
  return ( axis & 3 );
}

inline float BihNodeCompact::getExtremum(unsigned char i) const {
  return planes[i];
}

inline unsigned int BihNodeCompact::first() const {
  return primitives[0];
}

inline unsigned int BihNodeCompact::last() const {
  return primitives[1];
}

}

#endif
