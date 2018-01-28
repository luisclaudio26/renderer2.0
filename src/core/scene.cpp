#include "../../include/core/scene.h"
#include <glm/gtx/string_cast.hpp>
#include <cfloat>
#include <cstdio>

bool Scene::intersect(const Ray& r, Isect& target) const
{
  return tree.intersect(r, prims, target);
}

bool Scene::intersect(const Ray& r, float t) const
{
  return tree.intersect(r, prims, t);
}

void Scene::preprocess()
{
  //build kdtree
  AABB aabb;

  printf("Building tree...\n");
  tree.build(prims, aabb);
  printf("\tdone.\n");

  //store emissive primitives
  for(int i = 0; i < prims.size(); ++i)
    if( prims[i].material->is_emissive() )
    {
      emissive_area += prims[i].area();
      emissive.push_back(i);
    }

  printf("%lu emissive primitives\n", emissive.size());

  //compute environment sphere
  environment.c = (aabb.min + aabb.max) * 0.5f;
  environment.r = (environment.c - aabb.max).length();

  emissive_area += 12.566370f*environment.r*environment.r; //4PI = 12.566...
}

RGB Scene::sample_light(Vec3& pos, float& pdf, const Triangle** prim) const
{
  //TODO; handle the case where there are no
  //emissive primitives AND environment/infinite
  //lights

  //TODO: no PBRT, o light_pdf já é calculado em relação a ângulo sólido,
  //o que significa que (1) a distância da esfera de environment light
  //NÃO importa, já que não é necessário calcular termo de acoplamento
  //geométrico
  if( emissive.empty() )
  {
    pdf = 1.0f; //??
    prim = NULL;
    return RGB(0.f);
  }

  int i = rand() % emissive.size();
  float u = (float)rand() / RAND_MAX;
  float v = (float)rand() / RAND_MAX;
  float w = 1 - u - v;

  const Triangle& t = prims[emissive[i]];

  pos = u*t.v[0] + v*t.v[1] + w*t.v[2];
  pdf = 1.0f / t.area();
  *prim = &t;

  //emissivity * 1/pdf, where 1/pdf = 1/(1/size) = size
  return t.material->emissivity() * (float)emissive.size();
}
