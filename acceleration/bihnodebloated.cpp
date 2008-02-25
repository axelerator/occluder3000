#include "bihnodebloated.h"
#include <iostream>

using namespace Occluder;

unsigned int BihNode::leafCount() const {
  return countNodes(false);
}


unsigned int BihNode::nodeCount() const{
  return countNodes(true);
}

AvgInfo BihNode::avgTrisPerLeaf() const{
  if ( isLeaf() ) {
    const AvgInfo result = { 1, last() - first() + 1 };
    return result;
  }

  const AvgInfo left =  getChild(false).avgTrisPerLeaf();
  const AvgInfo right =  getChild(true).avgTrisPerLeaf();
  const unsigned int total = left.n + right.n;
  const AvgInfo result =  { total, (left.avg * ((double)left.n / total) + (right.avg * ( (double)right.n / total ))) };
  return result;
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
  std::cout << "avgTrisPerLeaf:" << avgTrisPerLeaf().avg << "\n";

  std::cout << std::endl;
}
