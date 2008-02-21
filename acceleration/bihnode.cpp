//
// C++ Implementation: BihNodeCompact
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bihnode.h"
#include <iostream>
using namespace Occluder;

unsigned int BihNodeCompact::leafCount() const {
  if ( isLeaf() )
    return 1;
  return this->getChild(0).leafCount() + this->getChild(1).leafCount();
}

unsigned int BihNodeCompact::nodeCount() const {
  if ( isLeaf() )
    return 1;
  const unsigned int lc = this->getChild(0).nodeCount();
  const unsigned int rc = this->getChild(1).nodeCount();
  return 1 + lc + rc;
/*  return 1 + this->getChild(0).nodeCount() + this->getChild(1).nodeCount();*/
}

double BihNodeCompact::avgTrisPerLeaf() const {
  if ( isLeaf() )
    return primitives[1] - primitives[0] + 1;
  return this->getChild(0).avgTrisPerLeaf() * 0.5  + this->getChild(1).avgTrisPerLeaf() * 0.5;
}

unsigned int BihNodeCompact::treeDepth() const {
  if ( isLeaf() )
    return 1;
  const unsigned int ld = getChild(0).treeDepth();
  const unsigned int rd = getChild(1).treeDepth();
  return 1 + ((ld > rd) ? ld : rd);
}

void BihNodeCompact::analyze() const {
  std::cout << "Nodecount:" << nodeCount() << "\n";
  std::cout << "Tree depth:" << treeDepth() << "\n";
  const unsigned int lc = leafCount();
  std::cout << "Leafcount:" << lc << "\n";
  std::cout << "avgTrisPerLeaf:" << avgTrisPerLeaf() << "\n";

  std::cout << std::endl;
}
