#include "../../include/material/mtl.h"

RGB MTL::sample(const Vec3& wi, const Vec3& wo,
                      const Vec3& normal, const Vec2& uv) const
{
  if( diff_tex.use_count() )
    return diff_tex->sample(uv);
  else return diffuse;

}
