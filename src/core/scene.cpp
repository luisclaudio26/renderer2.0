#include "../../include/core/scene.h"
#include <cfloat>

bool Scene::intersect(const Ray& r, Isect& target) const
{
  //TODO: kd-trees <3 <3 !!!
  target.t = FLT_MAX;

  for(auto p : prims)
  {
    Isect inter; p->intersect(r, inter);

    if(inter.is_valid() && inter.t < target.t)
      target = inter;

  }

  //invalidate isect if we intersected no primitive
  if(target.t == FLT_MAX) target.t = -1.0f;

  return target.is_valid();
}

void Scene::preprocess()
{
  //store emissive primitives
  for(auto p : prims)
    if(p->material->is_emissive()) emissive.push_back(p);

  //build kdtree
  tree.build(prims);
}
