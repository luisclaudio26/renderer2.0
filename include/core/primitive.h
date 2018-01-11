#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "intersection.h"
#include "geometry.h"
#include <memory>

class Primitive
{
private:
  Material::ptr material;

public:
  typedef std::shared_ptr<Primitive> ptr;

  virtual void intersect(const Ray& ray, Isect& isect) const = 0;
};

#endif
