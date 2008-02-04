//
// C++ Interface: kdtree
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KDTREE_H
#define KDTREE_H
#include <vector>
#include <simplevector.h>
#include <accelerationstruct.h>
#include <set>

/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdTree : public AccelerationStruct {
  public:
    KdTree(const Scene &scene);

    ~KdTree();

    virtual const RGBvalue trace ( RadianceRay& r, unsigned int depth ) const;
    virtual bool trace ( RayPacket& rp, unsigned int depth = 5 ) const;
    virtual const Intersection& getClosestIntersection(RadianceRay& r) const;
    virtual bool isBlocked(Ray& r) const;
    virtual void construct();
  private:
    struct KdTreenode  {
      unsigned char axis;
      float splitPos;
      union {
        KdTreenode *leftchild;
        SmallStaticArray<unsigned int> *prims;
      };
    };
    typedef KdTreenode KdTreenode;

    typedef struct {
      KdTreenode *node;
      float tMin;
      float tMax;
    } KdTreeStacknode;

    typedef struct {
      KdTreenode *node;
      __m128 tMin;
      __m128 tMax;
    } KdTreePacektStacknode;

    bool traverse(const KdTreenode& node, RadianceRay& r, float tMax ) const;
    bool traverseIterative( RadianceRay& r ) const;
    void traversePacket( RayPacket& rp ) const;
    bool traverseShadowIterative( Ray& r ) const;
    void subdivide ( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds, unsigned int depth );
    void deleteNode( KdTreenode &node );
    float calculatecost( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds,float splitpos );
    bool checkConsitency() const;
    bool checkConsRec(KdTreenode *node, std::set<unsigned int>& missing) const ;

    SimpleVector<KdTreenode> nodes;
};

#endif
