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
#ifdef VISUAL_DEBUGGER    
  #include "glwidget.h"
#endif
#include <set>
/**
	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
class KdTree : public AccelerationStruct {
  public:
    KdTree(const Scene &scene);

    ~KdTree();

    virtual const RGBvalue trace ( Ray& r, unsigned int depth );
    virtual void construct();
#ifdef VISUAL_DEBUGGER    
    virtual void draw ( GLWidget* context ) const;
    virtual void drawSchema ( GLWidget* context ) const;
    virtual void drawWithNames ( GLWidget* context ) const;
    virtual void keyReleaseEvent ( QKeyEvent* event );
    virtual void select ( int selected );
#endif
  private:
    struct KdTreenode  {
      unsigned char axis;
      float splitPos;
#ifdef VISUAL_DEBUGGER
      float bounds[6];
      unsigned int idx;
#endif
      union {
        KdTreenode *leftchild;
        SmallStaticArray<unsigned int> *prims;
      };
    };
    typedef KdTreenode KdTreenode;

    bool traverse(const KdTreenode& node, RadianceRay& r, float tMax );
    bool traverseIterative( RadianceRay& r , float tMin , float tMax );
    void subdivide ( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds, unsigned int depth );
    void deleteNode( KdTreenode &node );
    float calculatecost( KdTreenode &node, const std::vector<unsigned int> &prims, float *bounds,float splitpos );
    bool checkConsitency() const;
    bool checkConsRec(KdTreenode *node, std::set<unsigned int>& missing) const ;

    SimpleVector<KdTreenode> nodes;
#ifdef VISUAL_DEBUGGER    
    void drawContent(const KdTreenode& node, GLWidget* context, bool parentMarked)  const;
    const Vector3D drawTree(const KdTreenode& node, unsigned int depth, GLWidget* context ) const;
    unsigned int nodeId;
    unsigned int markednode ;
#endif

};

#endif
