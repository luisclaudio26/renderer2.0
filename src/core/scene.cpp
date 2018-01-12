#include "../../include/core/scene.h"

bool Scene::intersect(const Ray& r, Isect& target) const
{

}

void Scene::preprocess()
{
  //store emissive primitives
  for(auto p : prims)
    if(p->material->is_emissive()) emissive.push_back(p);
}
