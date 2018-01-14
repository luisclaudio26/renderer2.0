#include "../../include/integrator/pathtracer.h"

RGB Pathtracer::integrate(const Vec2& uv, const Scene& scene) const
{
  Isect isect;
  Ray r = scene.cam->getRay(uv);

  //if we had no intersection, sample the background
  if( !scene.intersect(r, isect) )
    return scene.bgd->sample(r);

  Vec3 dummy(0.0f);
  RGB tex = isect.material->sample(dummy, dummy, dummy, isect.uv);
  return tex;
}
