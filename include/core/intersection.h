#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "material.h"
#include "geometry.h"

//record info on an intersection event
class Isect
{
public:
  float t;
  Material::ptr mat;
  Vec3 normal;
  Vec2 uv; //UV coordinates of the intersection point
};

#endif
