#include "../../include/core/scene.h"
#include <cfloat>

bool Scene::intersect(const Ray& r, Isect& target) const
{
  //TODO: kd-trees <3 <3 !!!
  Isect closest; closest.t = FLT_MAX;

  for(auto p : prims)
  {
    Isect inter; p->intersect(r, inter);

    if(inter.is_valid() && inter.t < closest.t)
      closest = inter;
  }

  return closest.t < FLT_MAX;
}

void Scene::preprocess()
{
  //store emissive primitives
  for(auto p : prims)
    if(p->material->is_emissive()) emissive.push_back(p);
}
