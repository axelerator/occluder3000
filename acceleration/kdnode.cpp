//
// C++ Implementation: kdnode
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdnode.h"
#include <iostream>

using namespace Occluder;

KdNode::KdNode() {
}


KdNode::~KdNode() {
}


unsigned int KdNode::leafCount() const {
  if ( isLeaf() )
    return 1;
  return getChild(false).leafCount() + getChild(true).leafCount();
}

unsigned int KdNode::nodeCount() const {
  if ( isLeaf() )
    return 1;
  return 1 + getChild(false).nodeCount() + getChild(true).nodeCount();
}

AvgInfo KdNode::avgTrisPerLeaf() const {
  if ( isLeaf() ) {
    const AvgInfo result = { 1, leaf.primitiveCount };
    return result;
  }

  const AvgInfo left =  getChild(false).avgTrisPerLeaf();
  const AvgInfo right =  getChild(true).avgTrisPerLeaf();
  const unsigned int total = left.n + right.n;
  const AvgInfo result =  { total, (left.avg * ((double)left.n / total) + (right.avg * ( (double)right.n / total ))) };
  return result;
}

void KdNode::analyze() const {
  std::cout << "Nodecount:" << nodeCount() << "\n";
  std::cout << "Tree depth:" << treeDepth() << "\n";
  const unsigned int lc = leafCount();
  std::cout << "Leafcount:" << lc << "\n";
  std::cout << "avgTrisPerLeaf:" << avgTrisPerLeaf().avg << "\n";

  std::cout << std::endl;
}

unsigned int KdNode::treeDepth() const {
  if ( isLeaf() )
    return 1;
  else {
    const unsigned int lmax = getChild(false).treeDepth();
    const unsigned int rmax = getChild(true).treeDepth();
    return 1 + ( ( lmax < rmax ) ? rmax : lmax );
  }
}

/*inline*/ const KdNode& KdNode::getChild(bool right) const {
//   return this[ (right) ? ((innernode.typeAxisRightNode & 0x7FFFFFFC) >> 1 ): 1];
  if ( right ) {
    unsigned int *m = (unsigned int *)this;
    unsigned int offset = (innernode.typeAxisRightNode & 0x7FFFFFFC) >> 2;
    return *((KdNode*)(m + offset)); 
  }
    
  return this[1];
}



