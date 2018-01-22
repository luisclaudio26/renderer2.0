#include "../../include/integrator/pathtracer.h"
#include <glm/gtx/string_cast.hpp>

//TODO: read this from file
#define PPP 35
#define MAX_DEPTH 3

static RGB sample_path(int path_length, const Scene& scene,
                        const Isect& first_isect, const Ray& eye_ray)
{
  //"Base case" : path = 1 means we return the emissivity
  //of the first object encountered
  if( path_length == 1 )
    return first_isect.material->emissivity();

  //path throughput includes the computation of the path sampling
  //probability, as at each step we divide it by the probability
  //of sampling the direction of the next casted ray.

  //TODO: throughput tem que considerar distância ao olho
  //ângulo da normal/eye_ray e probabilidade de escolher a
  //primeira interseção
  RGB throughput(1.0f);

  //compute throughput of the path between the origin
  //and last but one vertex in the path.
  //Ray-In always points to the intersection point.
  //remember to invert it when computing BSDFs!!!
  Ray ri = eye_ray;
  Isect V_isect = first_isect;

  for(int i = 0; i < path_length-2; ++i)
  {
    //compute closest intersection of ri
    //and store it directly into V_isect
    //If this is the first ray, we already
    //know that V_isect = first_isect, so we
    //can skip this
    if(i > 0) scene.intersect(ri, V_isect);

    //if at this point we intersected no primitive, the contribution
    //of this path will then depend of the background
    if( !V_isect.is_valid() )
      return throughput * scene.bgd->sample( ri );

    //TODO: on the last iteration, we don't need/can't
    //sample BSDF, update throughput and ri, because
    //we will compute direct illumination in the next
    //step. as an advantage, ri will end up storing the
    //ray ENTERING the surface on the last intersection
    //point, so we can use it to compute the geometric
    //coupling term in the direct illumination step.
    if( i == path_length-3 ) break;

    //sample BSDF to get the next direction. we are importance sampling
    //the BSDF here, trying to build a path of high contribution!
    Vec3 wo; float wo_pdf; RGB brdf;
    V_isect.material->sample_BSDF(V_isect.uv, ri, V_isect.normal,
                                    wo, wo_pdf, brdf);

    //update throughput
    float cosO = std::fabs(glm::dot(wo, V_isect.normal));
    throughput *= cosO * brdf / wo_pdf;

    //ray for next iteration
    ri = Ray( ri(V_isect.t) + V_isect.normal*0.00001f, wo);
  }

  //TODO: there's a wrong detail: we must disable
  //backface culling, otherwise rays which go through
  //the objects will hit the light and be illuminated!

  //the last vertex lies on an emissive primitive, so
  //we sample the light sources (this will change once
  //we implement multiple importance sampling)
  //TODO: for specular materials, sample BSDF
  Vec3 light_pos; float light_pdf;
  RGB Le = scene.sample_light(light_pos, light_pdf);

  Vec3 V = ri(V_isect.t);
  float d = glm::length(light_pos - V);
  Vec3 wo = glm::normalize(light_pos - V);

  //check visibility. If not visible, path contribution is zero!
  //the outgoing ray intersects the light sample at t = d!
  if( scene.intersect(Ray(V+V_isect.normal*0.00001f, wo), d) )
    return RGB(0.f);

  //G: geometric coupling term
  float cosI = glm::dot(ri.d, V_isect.normal);
  float cosO = glm::dot(wo, V_isect.normal);
  float G = std::fabs(cosI*cosO) / (d*d);

  //this will cause problems with specular materials (i.e. delta BSDFs)
  RGB f = V_isect.material->sample(ri.d, wo, V_isect.normal, V_isect.uv);

  //add contribution of the last bounce to the throughput
  throughput *= (G/light_pdf) * f;

  return Le * throughput;
}

RGB Pathtracer::integrate(const Vec2& uv, const Scene& scene) const
{
  Isect first_isect;
  Ray eye_ray = scene.cam->getRay(uv);

  //if we had no intersection, sample the background
  if( !scene.intersect(eye_ray, first_isect) )
    return scene.bgd->sample(eye_ray);

  //No russian rouletting for now nor path reusing, we're
  //just estimating the contribution of the paths of
  //length i (L(p_i)), i <= max_length
  RGB total_radiance(0.0f);
  for(int i = 1; i <= MAX_DEPTH; ++i)
  {
    //contribution of path of length i
    //this is simply a monte carlo integration!
    RGB Li(0.0f);

    for(int j = 0; j < PPP; ++j)
    {
      //sample_path gives us the path radiance weighted by the path's
      //probability, i.e. f(X)/p(X), so we can add it directly to Li
      RGB path_radiance = sample_path(i, scene, first_isect, eye_ray);
      Li += path_radiance;
    }

    total_radiance += (Li / (float)PPP);
  }

  return total_radiance;
}
