//
// C++ Interface: kdnodebloated
//
// Description: 
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERKDNODEBLOATED_H
#define OCCLUDERKDNODEBLOATED_H
#include "list.h"
#include "bintree.h"


namespace Occluder {

/**
Legere layout for kdtree nodes

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdNodeBloated {
public:

    KdNodeBloated(const KdNodeBloated* left, const KdNodeBloated* right, unsigned char axis, float splitPos );

    KdNodeBloated( const List<unsigned int>& primitives );
    ~KdNodeBloated();


    bool isLeaf() const;
    unsigned int getAxis() const;
    const KdNodeBloated& getChild(unsigned int i) const;
    float getSplitpos() const;
    const List<unsigned int>& getPrimitives() const;

    void analyze() const;
    void print() const;


private:
    // info methods
    AvgInfo avgTrisPerLeaf() const;
    unsigned int leafCount() const;
    unsigned int nodeCount() const;
    unsigned int treeDepth() const;

  const bool isleaf;

  const float splitPos;
  const unsigned char axis;
  union {
    const KdNodeBloated *child[2];
    const List<unsigned int> *primitives;
  };

};


inline KdNodeBloated::KdNodeBloated(const KdNodeBloated* left, const KdNodeBloated* right, unsigned char axis, float splitPos ):
  isleaf(false),
  splitPos(splitPos),
  axis(axis) {
child[0] = left;
child[1] = right;
}


inline KdNodeBloated::KdNodeBloated( const List<unsigned int>& primitives ):
  isleaf(true),
  splitPos(0.0f),
  axis(3) {
child[0] = 0;
child[1] = 0;
this->primitives = new List<unsigned int>(primitives);
}

inline KdNodeBloated::~KdNodeBloated() {
  if ( isLeaf() ) {
    delete primitives;
  } else {
    delete child[0];
    delete child[1];
  }
}

inline bool KdNodeBloated::isLeaf() const {
  return isleaf;
}

inline unsigned int KdNodeBloated::getAxis() const {
  return axis;
}

inline const KdNodeBloated& KdNodeBloated::getChild(unsigned int i) const {
  return *child[i];
}

inline const List<unsigned int>& KdNodeBloated::getPrimitives() const {
  return *primitives;
}

inline float KdNodeBloated::getSplitpos() const {
  return splitPos;
}

}

#endif
