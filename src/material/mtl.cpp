#include "../../include/material/mtl.h"
#include <cstdlib>

RGB MTL::sample(const Vec3& wi, const Vec3& wo,
                      const Vec3& normal, const Vec2& uv) const
{
  if( diff_tex.use_count() )
    return diff_tex->sample(uv);
  else return diffuse;

}

void MTL::sample_BSDF(const Vec2& uv, const Ray& wi, const Vec3& normal,
                      Vec3& wo, float& wo_pdf, RGB& brdf) const
{
  //TODO: importance sample BRDFs
  brdf = RGB(0.318309f); //1/PI

  //uniform sample hemisphere
  float u1 = (float)rand()/RAND_MAX;
  float u2 = (float)rand()/RAND_MAX;

  float z = u1;
  float r = sqrtf(std::max(0.f, 1.f - z*z));
  float phi = 2*3.141592f*u2;
  float x = r * cos(phi);
  float y = r * sin(phi);
  wo = Vec3(x, y, z);

  if( glm::dot(wo, normal) < 0 ) wo = -wo;

  //PDF is constant
  wo_pdf = 0.159154943f; //1/2*PI
}
