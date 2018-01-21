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

  printf("%d emissive primitives\n", emissive.size());

  //build kdtree
  printf("Building tree...\n");
  tree.build(prims);
  printf("\tdone.\n");
}

RGB Scene::sample_light(Vec3& pos, float& pdf) const
{
  pdf = 1.0f;
  pos = prims[emissive[0]].v[0];
  return RGB(1.f, 0.f, 0.f);
}
