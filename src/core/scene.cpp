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

  float span = std::max(aabb.max[0]-aabb.min[0],
                        std::max(aabb.max[1]-aabb.min[1],
                                  aabb.max[2]-aabb.min[2]));
  environment.r = span * 1.44f; //TODO: why are primitives falling beyond
                                //the [min, max] range?!


  emissive_area += 12.566370f*environment.r*environment.r; //4PI = 12.566...
}

RGB Scene::sample_light(Vec3& pos, float& pdf) const
{
  //TODO; handle the case where there are no
  //emissive primitives AND environment/infinite
  //lights
  if( emissive.empty() ) return RGB(0.f);

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

RGB Scene::eval_environment(const Ray& r, Isect& isect) const
{
  Vec3 x = r.o - environment.c;
  float a = 1.0f; //glm::dot(r.d, r.d);
  float b = 2 * glm::dot(x, r.d);
  float c = glm::dot(x,x) - environment.r * environment.r;

  //as the ray's origin is always inside the sphere,
  //we need to compute the positive root only!
  float delta = b*b - 4*c;
  float t = (-b + sqrtf(delta)) * 0.5f;

  isect.normal = glm::normalize( r(t) - environment.c );
  isect.uv = Vec2(0.5f, 0.5f); //TODO: implement this correctly
  isect.t = t;
  isect.material = NULL;

  return RGB(0.3f, 0.3f, 0.3f);
}
