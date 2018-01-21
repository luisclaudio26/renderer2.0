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
    if( prims[i].material->is_emissive() )
    {
      emissive_area += prims[i].area();
      emissive.push_back(i);
    }

  printf("%d emissive primitives\n", emissive.size());

  //build kdtree
  printf("Building tree...\n");
  tree.build(prims);
  printf("\tdone.\n");
}

RGB Scene::sample_light(Vec3& pos, float& pdf) const
{
  //TODO; handle the case where there are no
  //emissive primitives
  float i = rand() % emissive.size();
  float u = (float)rand() / RAND_MAX;
  float v = (float)rand() / RAND_MAX;
  float w = 1 - u - v;

  const Triangle& t = prims[emissive[i]];

  pos = u*t.v[0] + v*t.v[1] + w*t.v[2];
  pdf = 1 / emissive_area;

  //TODO: sample primitive's emission profile
  return RGB(1.f, 0.f, 0.f);
}
