#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "primitive.h"
#include "material.h"
#include "texture.h"
#include "intersection.h"
#include "camera.h"
#include "background.h"

class Scene
{
public:
  //it will be EXTREMElY inefficient to store
  //vectors of pointers, as the objects will be
  //spread in memory and cache misses will happen
  //all the time. How to better store this?
  //TODO: what if we head a huge memory pool and
  //used it to store heterogeneous objects? once we
  //needed to extend it, we would copy everything
  //OR create a link to the next block. It would be
  //slow to create but, once created it, everything would
  //be well aligned and cache usage would maybe improve
  std::vector<Primitive::ptr> prims;
  std::vector<Material::ptr> materials;
  std::vector<Texture::ptr> textures;

  std::vector<Primitive::ptr> emissive;

  Camera::ptr cam;
  Background::ptr bgd;

  //TODO: kdtree

  bool intersect(const Ray& r, Isect& target) const;
  void preprocess();
};

#endif
