#include "../../include/integrator/pathtracer.h"
#include <glm/gtx/string_cast.hpp>

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
  Ray next_ri = eye_ray;
  Isect V_isect = first_isect;

  for(int i = 0; i < path_length-2; ++i)
  {
    ri = next_ri;

    //sample BSDF to get the next direction. we are importance sampling
    //the BSDF here, trying to build a path of high contribution!
    Vec3 wo; float wo_pdf; RGB brdf;
    V_isect.material->sample_BSDF(V_isect.uv, ri, V_isect.normal,
                                    wo, wo_pdf, brdf);

    //update throughput
    float cosO = std::fabs(glm::dot(wo, V_isect.normal));
    throughput *= cosO * brdf / wo_pdf;

    //ray for next iteration
    Ray next_ri( ri(V_isect.t) + V_isect.normal*0.00001f, wo);

    //compute closest intersection of ri
    //and store it directly into V_isect
    scene.intersect(next_ri, V_isect);

    //if at this point we intersected no primitive, the contribution
    //of this path will be zero. previously we would return the background
    //multiplied by the throughput, but this makes paths of length < i contribute
    //with lighting as paths of length = i, which introduce bias.
    if( !V_isect.is_valid() ) return RGB(0.f);
  }

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
  RGB total_radiance(0.0f); float path_p = 1.0f;

  float eval_probs[] = {0.9f, 0.8f, 0.4f, 0.2f, 0.1f};

  for(int i = 1; ; ++i)
  {
    //russian roulette
    float q = (float)rand()/RAND_MAX;
    float p = i > 5 ? 0.01f : eval_probs[i];
    if(q > p) break;
    else path_p *= 1.0f / p;

    //sample_path gives us the path radiance weighted by the path's
    //probability, i.e. f(X)/p(X), so we can add it directly to Li
    RGB path_radiance = sample_path(i, scene, first_isect, eye_ray);

    total_radiance += path_p * path_radiance;
  }

  return total_radiance;
}
