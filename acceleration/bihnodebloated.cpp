#include "bihnodebloated.h"
#include <iostream>

using namespace Occluder;

unsigned int BihNode::leafCount() const {
  return countNodes(false);
}


unsigned int BihNode::nodeCount() const{
  return countNodes(true);
}

double BihNode::avgTrisPerLeaf() const{
  if ( isLeaf() ) 
    return last() - first() + 1;
  return getChild(0).avgTrisPerLeaf() * 0.5 + getChild(1).avgTrisPerLeaf() * 0.5;
}


unsigned int BihNode::countNodes(bool countInnersToo) const {
  if ( isLeaf() )
    return 1;
  return ((countInnersToo)?1:0) + getChild(0).countNodes(countInnersToo) + getChild(1).countNodes(countInnersToo);
}

unsigned int BihNode::treeDepth() const {
  if ( isLeaf() )
    return 1;
  else {
    const unsigned int lmax = getChild(0).treeDepth();
    const unsigned int rmax = getChild(1).treeDepth();
    return 1 + ( ( lmax < rmax ) ? rmax : lmax );
  }
}

void BihNode::analyze() const {
  std::cout << "Nodecount:" << nodeCount() << "\n";
  std::cout << "Tree depth:" << treeDepth() << "\n";
  const unsigned int lc = leafCount();
  std::cout << "Leafcount:" << lc << "\n";
  std::cout << "avgTrisPerLeaf:" << avgTrisPerLeaf() << "\n";

  std::cout << std::endl;
}
