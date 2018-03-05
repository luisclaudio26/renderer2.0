#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "geometry.h"
#include "material.h"
#include "intersection.h"

class Triangle
{
public:
  Vec3 v[3]; Vec2 uv[3];

  Material::ptr material;

  void transform(const Mat4& T)
  {
    for(int i = 0; i < 3; ++i)
      v[i] = Vec3(T * Vec4(v[i], 1.0f));
  }

  void intersect(const Ray& ray, Isect& isect, bool bf_cull = true) const;
  void aabb(AABB& target) const;
  float area() const;
};

#endif
