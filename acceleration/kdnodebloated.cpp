//
// C++ Implementation: kdnodebloated
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kdnodebloated.h"
#include <iostream>

using namespace Occluder;

unsigned int KdNodeBloated::leafCount() const {
  if ( isLeaf() )
    return 1;
  return getChild(false).leafCount() + getChild(true).leafCount();
}

unsigned int KdNodeBloated::nodeCount() const {
  if ( isLeaf() )
    return 1;
  return 1 + getChild(false).nodeCount() + getChild(true).nodeCount();
}

AvgInfo KdNodeBloated::avgTrisPerLeaf() const {
  if ( isLeaf() ) {
    const AvgInfo result = {1,primitives->size()};
    return result;
  }

  const AvgInfo left =  getChild(false).avgTrisPerLeaf();
  const AvgInfo right =  getChild(true).avgTrisPerLeaf();
  const unsigned int total = left.n + right.n;
  const AvgInfo result =  { total, (left.avg * ((double)left.n / total) + (right.avg * ( (double)right.n / total ))) };
  return result;
}

void KdNodeBloated::analyze() const {
  std::cout << "Nodecount:" << nodeCount() << "\n";
  std::cout << "Tree depth:" << treeDepth() << "\n";
  const unsigned int lc = leafCount();
  std::cout << "Leafcount:" << lc << "\n";
  std::cout << "avgTrisPerLeaf:" << avgTrisPerLeaf().avg << "\n";
  print();
  std::cout << std::endl;
}

unsigned int KdNodeBloated::treeDepth() const {
  if ( isLeaf() )
    return 1;
  else {
    const unsigned int lmax = getChild(false).treeDepth();
    const unsigned int rmax = getChild(true).treeDepth();
    return 1 + ( ( lmax < rmax ) ? rmax : lmax );
  }
}

void KdNodeBloated::print() const {

  if ( isLeaf() ) {
    std::cout << "[";
    for ( unsigned int i = 0; i < primitives->size(); ++i )
      std::cout << (*primitives)[i] << ",";
    std::cout << "]";
  } else {
    std::cout << "(";
    getChild(false).print();
    std::cout << ")" << splitPos << "{" << getAxis() << "}"  << "(";
    getChild(true).print();
    std::cout << ")";


  }
}
