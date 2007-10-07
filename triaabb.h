#ifndef TRIAABB
#define TRIAABB


class TriAABBIntersect {
  public:
    static bool intersects(const Vector3D& boxOrigin, const Vector3D& boxhalfsize, const Triangle& tri);
};

#endif
