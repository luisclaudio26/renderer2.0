#include "../../include/core/triangle.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

float Triangle::area() const
{
  //TODO: precompute this
  Vec3 v0v1 = this->v[1] - this->v[0];
  Vec3 v0v2 = this->v[2] - this->v[0];

  Vec3 cross = glm::cross(v0v1, v0v2);

  return std::fabs(glm::length(cross)) * 0.5f;
}

void Triangle::intersect(const Ray& ray, Isect& isect, bool bf_cull) const
{
  isect.t = -1.0f;

  // Möller-Trumbore algorithm, as described in
  // https://www.scratchapixel.com/lessons/3d-basic-rendering/
  // ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
  Vec3 v0v1 = this->v[1] - this->v[0];
  Vec3 v0v2 = this->v[2] - this->v[0];
  Vec3 pvec = glm::cross(ray.d, v0v2);
  float det = glm::dot(v0v1, pvec);

  //TODO: backface culling
  if(bf_cull && det < 0.00000001) return;
  else if(!bf_cull && fabs(det) < 0.00000001 ) return;

  float invDet = 1 / det;

  Vec3 tvec = ray.o - this->v[0];
  float u = glm::dot(tvec, pvec) * invDet;
  if (u < 0 || u > 1) return;

  Vec3 qvec = glm::cross(tvec, v0v1);
  float v = glm::dot(ray.d, qvec) * invDet;
  if (v < 0 || u + v > 1) return;

  isect.t = glm::dot(v0v2, qvec) * invDet;
  isect.normal = glm::normalize(glm::cross(v0v1, v0v2));
  isect.material = this->material;
  isect.uv = u*uv[1] + v*uv[2] + (1-u-v)*uv[0];
}

void Triangle::aabb(AABB& target) const
{
  for(int i = 0; i < 3; ++i)
  {
    target.max[i] = std::fmax(v[0][i], std::fmax(v[1][i], v[2][i]));
    target.min[i] = std::fmin(v[0][i], std::fmin(v[1][i], v[2][i]));
  }
}
