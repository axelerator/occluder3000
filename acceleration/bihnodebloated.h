#ifndef OCCLUDERBIHNODE_H
#define OCCLUDERBIHNODE_H
#include "bintree.h"

namespace Occluder {

/**
  This is the bloated version of a bihnode. It works exactly like the compressed version but
  stores data a bit more legere. Its purpose is to demonstrate how the BIH and to measure
  performance vs. the compressed version. I very well know this is everything else but perfect
  OO-Design, but it's done so that this version doest suck completely performancewise.
 **/
class BihNode {
    public:
      /**
        This constructor will create a leafnode
       **/
      BihNode(unsigned int start, unsigned int end);
      /**
        This constructir will create an inner node.
       **/
      BihNode(BihNode *left, BihNode *right, float leftMax, float rightMin, unsigned char axis);
      
      ~BihNode();
      bool isLeaf() const;

      /**
        Since the Bih is a binary tree, every inner node has two children
        which can be retrieved via this method.
        @param i which of the two children should be returned (0,1)
        @return the i-th child of this node.(undefined in case of leaf, or i > 1)
       **/
      const BihNode& getChild(unsigned char i) const;
      float getPlane(bool right) const;
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
      unsigned int countNodes(bool countInnersToo) const;

      bool isleaf;
      unsigned char axis;
      union {
        float planes[2];
        unsigned int primitives[2];
      };
      BihNode* children[2];
};

inline BihNode::BihNode(unsigned int start, unsigned int end) {
  primitives[0] = start;
  primitives[1] = end;
  isleaf = true;
  children[0] = children[1] = 0;
  axis = 3;
}

inline BihNode::BihNode(BihNode *left, BihNode *right, float leftMax, float rightMin, unsigned char axis) {
  children[0] = left;
  children[1] = right;
  planes[0] = leftMax;
  planes[1] = rightMin;
  this->axis = axis;
  isleaf = false;
}

inline BihNode::~BihNode() {
  if ( ! this->isLeaf() ) {
    delete children[0];
    delete children[1];
  }
}


inline bool BihNode::isLeaf() const {
  return isleaf;
}


inline const BihNode& BihNode::getChild(unsigned char i) const {
  return *children[i];
}


inline float BihNode::getPlane(bool right) const {
  return planes[right ? 1 : 0];
}


inline unsigned char BihNode::getAxis() const {
  return axis;
}

inline unsigned int BihNode::first() const {
  return primitives[0];
}

inline unsigned int BihNode::last() const {
  return primitives[1];
}

inline float BihNode::getExtremum(unsigned char i) const {
  return planes[i];
}

}
#endif
