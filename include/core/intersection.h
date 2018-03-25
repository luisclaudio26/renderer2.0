#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "geometry.h"

//forward declare triangle to avoid circularity, as triangle itself
//needs to know Isect for its intersect() method
class Triangle;

class Isect
{
public:
  const Triangle* tri; //DO NOT DELETE THIS POINTER!!!
  float t;
  Vec2 uv; //UV coordinates of the intersection point
  Vec3 normal;
  Mat3 local2world;

  //TODO: are t = INF intersections valid?
  bool is_valid() const { return t > 0 && t < FLT_MAX; }
};

#endif
