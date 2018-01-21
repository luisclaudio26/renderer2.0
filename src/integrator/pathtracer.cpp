#include "../../include/integrator/pathtracer.h"
#include <glm/gtx/string_cast.hpp>

//TODO: read this from file
#define PPP 35
#define MAX_DEPTH 3

static RGB sample_path(int path_length, const Scene& scene,
                        const Isect& first_isect, const Ray& eye_ray)
{
  //path throughput includes the computation of the path sampling
  //probability, as at each step we divide it by the probability
  //of sampling the direction of the next casted ray.
  RGB throughput(1.0f);

  //compute throughput of path between origin and last but one
  //vertex in the path
  Isect cur_isect = first_isect;
  Vec3 cur_vertex = eye_ray(first_isect.t);
  Ray cur_ray = eye_ray; //TODO: maybe this can be precomputed

  for(int i = 0; i < path_length-1; ++i)
  {
    //if at this point we intersected no primitive, the contribution
    //of this path will then depend of the background
    if( !cur_isect.is_valid() )
      return throughput * scene.bgd->sample( cur_ray );

    //sample BSDF to get the next direction. we are importance sampling
    //the BSDF here, trying to build a path of high contribution!
    Vec3 wo; float wo_pdf; RGB brdf;
    cur_isect.material->sample_BSDF(cur_isect.uv, cur_ray, cur_isect.normal,
                                    wo, wo_pdf, brdf);

    //update throughput
    //throughput *= glm::dot(wo, cur_isect.normal) * brdf / wo_pdf;
    throughput *= glm::dot(wo, cur_isect.normal) * brdf;

    //compute closest intersection of next_ray
    //and store it directly into cur_isect
    Ray next_ray(cur_vertex + cur_isect.normal * 0.00001f, wo);
    scene.intersect(next_ray, cur_isect);

    //update stuff for next iteration
    cur_ray = next_ray;
    cur_vertex = cur_ray(cur_isect.t);
  }

  //the last vertex lies on an emissive primitive, so
  //we sample the light sources (this will change once
  //we implement multiple importance sampling)
  Vec3 light_pos; float light_pdf;
  RGB Le = scene.sample_light(light_pos, light_pdf);

  //update throughput with probability computed
  //not using BSDF sampling, but actual area sampling
  //float coupling = dot dot ...

  //get acual emission spectrum from the last primitive

  return throughput * Le;
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
