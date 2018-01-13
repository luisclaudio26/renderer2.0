#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../core/primitive.h"
#include "../core/geometry.h"

class Triangle : public Primitive
{
public:
  Vec3 v[3];

  void transform(const Mat4& T)
  {
    for(int i = 0; i < 3; ++i)
      v[i] = T * Vec4(v[i], 1.0f);
  }

  void intersect(const Ray& ray, Isect& isect) const override;
  void aabb(AABB& target) const override;
};

#endif
