#include "../../include/core/scene.h"
#include <cfloat>
#include <cstdio>

bool Scene::intersect(const Ray& r, Isect& target) const
{
  return tree.intersect(r, prims, target);
}

void Scene::preprocess()
{
  //store emissive primitives
  for(int i = 0; i < prims.size(); ++i)
    if( prims[i].material->is_emissive() ) emissive.push_back(i);

  //build kdtree
  printf("Building tree...\n");
  tree.build(prims);
  printf("\tdone.\n");
}
