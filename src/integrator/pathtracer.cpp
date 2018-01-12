#include "../../include/integrator/pathtracer.h"

RGB Pathtracer::integrate(const Vec2& uv, const Scene& scene) const
{
  Isect isect;
  Ray r = scene.cam->getRay(uv);

  //if we had no intersection, sample the background
  if( !scene.intersect(r, isect) )
    return scene.bgd->sample(r);

  //if we any intersection, return normal
  return (isect.normal+RGB(1.0f))*0.5f;
}
