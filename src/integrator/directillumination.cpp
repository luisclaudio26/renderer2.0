#include "../include/integrator/directillumination.h"

static RGB sample_light(const Scene& scene, const Isect& isect,
                          const Ray& eye_ray, float& pdf)
{
  pdf = 0.0f;

  //sample light source. SCENE is responsible for importance sampling
  //lights according to radiance
  Vec3 light_pos; float light_pdf; const Triangle* tri = NULL;
  RGB Le = scene.sample_light(light_pos, light_pdf, &tri);

  Vec3 V = eye_ray(isect.t);
  Vec3 v2l = light_pos - V;
  float dist = glm::length(v2l);
  Vec3 wo = glm::normalize(v2l);

  //check visibility. If not visible, path contribution is zero!
  //the outgoing ray intersects the light sample at t = d!
  //We KNOW that the ray will intersect at least the light, so we don't need
  //to assert that it hasn't escaped
  Ray shadow(V+isect.normal*0.00001f, wo); Isect light_isect;
  scene.intersect( shadow, light_isect );

  //primitive is occluded!
  //TODO: handle the case where no intersection is found? Is it possible?
  if( !light_isect.is_valid() ||
      (light_isect.is_valid() && light_isect.tri != tri) )
      return RGB(0.f);

  //cosine between light surface normal and the incoming
  //ray (the one that exits the surface)
  float cosSNl = glm::dot(-wo, light_isect.normal);
  float r2 = dist*dist;

  //if ray is perpendicular to the normal, cosSNl = 0.
  //this makes pdf = INF, but apparently this is no problem
  //because k/INF = 0, so this sample won't be actually used

  //convert from area to solid angle probability
  //pdf = light_pdf * (r2 / cosSNl);
  pdf = light_pdf;

  //this will cause problems with specular materials (i.e. delta BSDFs)
  RGB f = isect.tri->material->sample(-eye_ray.d, wo, isect.normal, isect.uv);
  float cosSNi = glm::dot(wo, isect.normal);

  //contribution of this sample
  return Le * f * cosSNi;
}

static RGB sampleAllLights(const Scene& scene, const Isect& isect, const Vec3& wo)
{
  RGB out(0.0f);

  


  return out;
}

RGB DirectIllumination::integrate(const Vec2& uv, const Scene& scene) const
{
  Isect first_isect;
  Ray eye_ray = scene.cam->getRay(uv);

  //if we had no intersection, sample the background
  if( !scene.intersect(eye_ray, first_isect) )
    return scene.bgd->sample(eye_ray);

  //we had an intersection. compute lighting at this point
  RGB L = first_isect.tri->material->emissivity(); //do non-emissive objects return 0,0,0?

  //compute irradiance due to incident
  Vec3 p = eye_ray(first_isect.t);
  L += sampleAllLights(scene, first_isect, -eye_ray.d);

  return L;
}
