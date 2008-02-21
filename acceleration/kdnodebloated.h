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



namespace Occluder {

/**
Legere layout for kdtree nodes

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdNodeBloated {
public:

    KdNodeBloated(const KdNodeBloated* left, const KdNodeBloated* right, unsigned char axis, float splitPos );

    KdNodeBloated( const List<unsigned int> *primitives );
    ~KdNodeBloated();


    bool isLeaf() const;
    unsigned int getAxis() const;
    const KdNodeBloated& getChild(unsigned int i) const;
    const List<unsigned int>& getPrimitives() const;

private:


  const bool isleaf;

  const float splitPos;
  const unsigned char axis;
  union {
    const KdNodeBloated *child[2];
    const List<unsigned int> *primitives;
  };

};


KdNodeBloated::KdNodeBloated(const KdNodeBloated* left, const KdNodeBloated* right, unsigned char axis, float splitPos ):
  isleaf(false),
  splitPos(splitPos),
  axis(axis) {
child[0] = left;
child[1] = right;
}


KdNodeBloated::KdNodeBloated( const List<unsigned int> *primitives ):
  isleaf(true),
  splitPos(0.0f),
  axis(3),
  primitives(primitives) {
child[0] = 0;
child[1] = 0;
}

KdNodeBloated::~KdNodeBloated() {}



bool KdNodeBloated::isLeaf() const {
  return isleaf;
}

unsigned int KdNodeBloated::getAxis() const {
  return axis;
}

const KdNodeBloated& KdNodeBloated::getChild(unsigned int i) const {
  return *child[i];
}

const List<unsigned int>& KdNodeBloated::getPrimitives() const {
  return *primitives;
}


}

#endif
