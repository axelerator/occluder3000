//
// C++ Interface: kdnode
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDNODE_H
#define OCCLUDERKDNODE_H
#include <assert.h>
#include "bintree.h"

namespace Occluder {

/**
Compact representation of a kd-tree node.

  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdNode {
public:
    KdNode();
    ~KdNode();

    bool isLeaf() const;

    // methods for inner nodes
    unsigned char getAxis() const;
    const KdNode& getChild(bool right) const;
    float getSplitPos() const;

    // methods for leafnode
    typedef struct {
      const unsigned int *start;
      const unsigned int length;
    } IndexList;

    IndexList getPrimitiveList() const;

    // static methods
    static void makeLeaf(unsigned int *mem, unsigned int primitiveCount);
    static void makeInnernode(unsigned int *mem, unsigned char axis, float splitPos, unsigned int offsetToRightNode);

    // info methods
    unsigned int leafCount() const;
    unsigned int nodeCount() const;
    AvgInfo avgTrisPerLeaf() const;
    void analyze() const;
    unsigned int treeDepth() const;

    void print() const;
private:
  typedef struct {
    float splitPos;
    unsigned int typeAxisRightNode;
  } Innernode;

  typedef struct {
    unsigned int primitiveCount; // number of primitives followed by this leaf
    unsigned int type;
  } Leafnode;


  union {
    Innernode innernode;
    Leafnode leaf;
    unsigned int firstPrimitive;
  };

};
// ----------------------- implementation of inlined methods ------------------------------------------

inline bool KdNode::isLeaf() const {
 return (( innernode.typeAxisRightNode & (unsigned int )(1<<31)) == 0);
}

inline unsigned char KdNode::getAxis() const {
  return ( innernode.typeAxisRightNode & 0x3);
}



inline float KdNode::getSplitPos() const {
  assert(!isLeaf());
  return innernode.splitPos;
}

inline KdNode::IndexList KdNode::getPrimitiveList() const {
  assert( isLeaf() );
  IndexList result = { &(this[1].firstPrimitive), leaf.primitiveCount };
  return result;
}

inline void KdNode::makeLeaf(unsigned int *mem, unsigned int primitiveCount) {
  KdNode &node = *((KdNode *) mem);
  node.leaf.type = 0;
  node.leaf.primitiveCount = primitiveCount;
}

inline void KdNode::makeInnernode(unsigned int *mem, unsigned char axis, float splitPos, unsigned int offsetToRightNode) {
  KdNode &node = *((KdNode *) mem);
  node.innernode.typeAxisRightNode = offsetToRightNode;
  node.innernode.typeAxisRightNode |= 0x80000000;
  node.innernode.typeAxisRightNode |= axis;
  node.innernode.splitPos = splitPos;
}


}

#endif
