#include "../../include/core/scene.h"
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
  float longest = -FLT_MAX;
  for(int i = 0; i < 3; ++i)
  {
    float span = aabb.max[i] - aabb.min[i];
    if( span > longest ) longest = span;
  }

  environment.c = (aabb.min + aabb.max) * 0.5f;
  environment.r = longest * 0.5f;
  emissive_area += 12.566370f*environment.r*environment.r; //4PI = 12.566...
}

RGB Scene::sample_light(Vec3& pos, float& pdf) const
{
  //TODO; handle the case where there are no
  //emissive primitives AND environment/infinite
  //lights
  float i = rand() % emissive.size() + 1;
  RGB out;

  //the last primitive is the environment light
  if( i == emissive.size() )
  {
    //sample point on the sphere surface
    float u = 2.0f * ((float)rand()/RAND_MAX) - 1.0f;
    float v = 2.0f * ((float)rand()/RAND_MAX) - 1.0f;
    float w = 2.0f * ((float)rand()/RAND_MAX) - 1.0f;

    Vec3 d = glm::normalize( Vec3(u, v, w) );
    pos = environment.c + environment.r * d;
    out = bgd->sample( Ray(environment.c, d) );
  }
  else
  {
    float u = (float)rand() / RAND_MAX;
    float v = (float)rand() / RAND_MAX;
    float w = 1 - u - v;

    const Triangle& t = prims[emissive[i]];

    pos = u*t.v[0] + v*t.v[1] + w*t.v[2];
    out = t.material->emissivity();
  }

  pdf = 1 / emissive_area;
  return out;
}
