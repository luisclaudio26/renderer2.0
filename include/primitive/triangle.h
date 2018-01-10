#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../core/primitive.h"
#include "../core/geometry.h"

class Triangle : public Primitive
{
public:
  Vec3 v[3];
  int material;

  Triangle operator*(const Mat4& T) const
  {
    Triangle out;
    for(int i = 0; i < 3; ++i)
      out.v[i] = T * Vec4(v[i], 1.0f);
    return out;
  }

  void intersect(const Ray& ray, Isect& isect) const override {}
};

#endif
