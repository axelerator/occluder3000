//
// C++ Interface: bvhnodesimple
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OCCLUDERBVHNODESIMPLE_H
#define OCCLUDERBVHNODESIMPLE_H

#include "aabb.h"
#include "intersection.h"
namespace Occluder {

/**
Implementation of a node for the @see BvhSimple

  @author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class BvhNodeSimple {
  public:
    BvhNodeSimple();
    virtual ~BvhNodeSimple() {};

    /**
      fits the aabb of this nodes to the aabbs of the children/objects
     **/
    virtual const AABB update() = 0;

    // info methods
    virtual unsigned int leafCount() const = 0;
    virtual unsigned int treeDepth() const = 0;

    virtual const Intersection getFirstIntersection(const RaySegment& ray) const = 0;
};


class BvhSimpleLeaf : public BvhNodeSimple {
  public:
    BvhSimpleLeaf(const Primitive& primitive);
    virtual ~BvhSimpleLeaf() {};
    /**
      @return AABB of contained object
     **/
    virtual const AABB update();

    // info methods
    virtual unsigned int leafCount() const;
    virtual unsigned int treeDepth() const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;
  private:
    /**
      Index of the contained primitive
    **/
    const Primitive& primitive;
};

class BvhSimpleInner : public BvhNodeSimple {
  public:
    BvhSimpleInner(BvhNodeSimple *left, BvhNodeSimple *right);
    virtual ~BvhSimpleInner();
    /**
      fits the aabb of this nodes to the aabbs of the children
      @return updated AABB of this node
     **/
    virtual const AABB update();
    // info methods
    virtual unsigned int leafCount() const;
    virtual unsigned int treeDepth() const;
    virtual const Intersection getFirstIntersection(const RaySegment& ray) const;

  private:
    /**
      Childnodes
     **/
    BvhNodeSimple *left, *right;

    /**
      Bounding Box for this node
     **/
    AABB aabb;
};


}

#endif
