#include "../../include/integrator/pathtracer.h"
#include <glm/gtx/string_cast.hpp>

static RGB sample_light(const Scene& scene, const Isect& isect,
                          const Ray& last_ray, float& pdf)
{
  pdf = 0.0f;

  //sample light source. SCENE is responsible for importance sampling
  //lights according to radiance
  Vec3 light_pos; float light_pdf; const Triangle* tri = NULL;
  RGB Le = scene.sample_light(light_pos, light_pdf, &tri);

  Vec3 V = last_ray(isect.t);
  Vec3 v2l = light_pos - V;
  float dist = glm::length(v2l);
  Vec3 wo = glm::normalize(v2l);

  //check visibility. If not visible, path contribution is zero!
  //the outgoing ray intersects the light sample at t = d!
  //We KNOW that the ray will intersect at least the light, so we don't need
  //to assert that the ray hasn't escaped
  Ray shadow(V+isect.normal*0.00001f, wo); Isect light_isect;
  scene.intersect( shadow, light_isect );

  //primitive is occluded!
  if( light_isect.tri != tri ) return RGB(0.f);

  //G: geometric coupling term
  //TODO: no PBRT as luzes são amostradas e depois a PDF é
  //transformada para ângulo sólido, o que significa que não
  //é necessário computar termo de acoplamento geométrico.
  //Isso facilita a implementação do integrador, pois o último
  //bounce não tem que ser diferente dos anteriores
  float cosSNl = glm::dot(-wo, light_isect.normal); //Shadow ray and
                                                    //Normal at Light surface
  float r2 = dist*dist;

  //convert from area to solid angle probability
  pdf = light_pdf * (r2 / cosSNl);

  //this will cause problems with specular materials (i.e. delta BSDFs)
  RGB f = isect.tri->material->sample(last_ray.d, wo, isect.normal, isect.uv);
  float cosSNi = glm::dot(wo, isect.normal);

  //contribution of this sample
  return Le * f * cosSNi;
}

static RGB sample_bsdf(const Scene& scene, const Isect& isect,
                        const Ray& last_ray, float& pdf)
{
  //sample BSDF of the intersection
  Vec3 bsdf_dir; float bsdf_pdf; RGB f;
  isect.tri->material->sample_BSDF(isect.uv, -last_ray, isect.normal,
                                bsdf_dir, bsdf_pdf, f);

  pdf = bsdf_pdf;

  //trace ray in this new direction and check whether we intersect
  //any light source
  Ray new_ray(last_ray(isect.t)+isect.normal*0.000001f, bsdf_dir);
  Isect new_isect; RGB Le(0.0f);

  if( scene.intersect(new_ray, new_isect) )
  {
    //we could just set Le = emissivity and the results
    //would be correct if emissivity = 0.0f, but we don't
    //want to go through all the computations in the end
    if( new_isect.tri->material->is_emissive() )
      Le = new_isect.tri->material->emissivity();
    else return RGB(0.f);
  }
  else
  {
    //no intersection means that the ray escaped the scene
    //and thus we must sample the environment light
    Le = scene.bgd->sample(new_ray);
  }

  float cosO = glm::dot(new_ray.d, isect.normal);

  //return contribution of this sample
  return Le * f * cosO;
}

static RGB sample_path(int path_length, const Scene& scene,
                        const Isect& first_isect, const Ray& eye_ray)
{
  //"Base case" : path = 1 means we return the emissivity
  //of the first object encountered
  if( path_length == 1 )
    return first_isect.tri->material->emissivity();

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
    //sample BSDF to get the next direction. we are importance sampling
    //the BSDF here, trying to build a path of high contribution!
    Vec3 wo; float wo_pdf; RGB brdf;
    V_isect.tri->material->sample_BSDF(V_isect.uv, ri, V_isect.normal,
                                        wo, wo_pdf, brdf);

    //update throughput
    float cosO = std::fabs(glm::dot(wo, V_isect.normal));
    throughput *= cosO * brdf / wo_pdf;

    //ray for next iteration
    next_ri = Ray( ri(V_isect.t) + V_isect.normal*0.00001f, wo);

    //compute closest intersection of ri
    //and store it directly into V_isect
    if( !scene.intersect(next_ri, V_isect) ) return RGB(0.f);
    else ri = next_ri;

    //if at this point we intersected no primitive, the contribution
    //of this path will be zero. previously we would return the background
    //multiplied by the throughput, but this makes paths of length < i contribute
    //with lighting as paths of length = i, which introduce bias.
    //if( !V_isect.is_valid() ) return RGB(0.f);
  }

  //Light sampling
  float light_pdf;
  RGB DI = sample_light(scene, V_isect, ri, light_pdf);

  //BSDF sampling
  float bsdf_pdf;
  RGB BSDF = sample_bsdf(scene, V_isect, ri, bsdf_pdf);

  //multiple importance sampling using Balance heuristics
  //TODO: review this! PBRT implementation doesn't match
  //the definition of MIS, which is:
  // f(X)g(X)w(X)/p(X) for the contribution of sample X.
  //Why aren't we multiplying the two f and g, or are we doing
  //this implicitly? If we are doing this implicitly, this
  //estimate is correct (and it actually looks good)
  RGB total = (DI + BSDF) / (light_pdf + bsdf_pdf);

  //printf("%f, %s ---- %f, %s -> %s\n", glm::to_string(DI).c_str(), light_pdf, glm::to_string(BSDF).c_str(), bsdf_pdf, glm::to_string(total).c_str());

  return total * throughput;
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
