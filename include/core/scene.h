#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "material.h"
#include "texture.h"
#include "intersection.h"
#include "camera.h"
#include "background.h"
#include "../kdtree/kdtree.h"
#include "triangle.h"

class Scene
{
private:


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
  std::vector<Triangle> prims;
  std::vector<Material::ptr> materials;
  std::vector<Texture::ptr> textures;

  std::vector<int> emissive;
  float emissive_area;

  Camera::ptr cam;
  Background::ptr bgd;

  KdTree tree;

  bool intersect(const Ray& r, Isect& target) const;
  RGB sample_light(Vec3& pos, float& pdf) const;
  void preprocess();

  Scene() : emissive_area(0.f) {}
};

#endif
