#include "../../include/material/mtl.h"
#include <cstdlib>

RGB MTL::sample(const Vec3& wi, const Vec3& wo,
                      const Vec3& normal, const Vec2& uv) const
{
  if( diff_tex.use_count() )
    return diff_tex->sample(uv) * 0.318309f;
  else return diffuse * 0.318309f;
}

void MTL::sample_BSDF(const Vec2& uv, const Ray& wi, const Isect& isect,
                      Vec3& wo, float& wo_pdf, RGB& brdf) const
{
  //TODO: importance sample BRDFs
  if( diff_tex.use_count() ) brdf = diff_tex->sample(uv) * 0.318309f; //1/PI
  else brdf = diffuse * 0.318309f;

  //TEST: shoot rays straight up
  wo_pdf = 1.0f;
  wo = isect.local2world * glm::normalize(Vec3(0.0f, 1.0f, 1.0f));

  //uniform sample hemisphere
  /*
  float u1 = (float)rand()/RAND_MAX;
  float u2 = (float)rand()/RAND_MAX;

  float z = u1;
  float r = sqrtf(std::max(0.f, 1.f - z*z));
  float phi = 2*3.141592f*u2;
  float x = r * cos(phi);
  float y = r * sin(phi);
  wo = Vec3(x, y, z);

  if( glm::dot(wo, isect.normal) < 0 ) wo = -wo;

  //PDF is constant
  wo_pdf = 0.159154943f; //1/2*PI
  */

  //cosine sample hemisphere
}
