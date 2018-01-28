#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "material.h"
#include "geometry.h"

//forward declare triangle to avoid circularity, as triangle itself
//needs to know Isect for its intersect() method
class Triangle;

class Isect
{
public:
  const Triangle* tri; //DO NOT DELETE THIS POINTER!!!
  float t;
  Vec3 normal;
  Vec2 uv; //UV coordinates of the intersection point

  //TODO: are t = INF intersections valid?
  bool is_valid() const { return t > 0 && t < FLT_MAX; }
};

#endif
