#include "../../include/core/integrator.h"
#include "../../include/integrator/pathtracer.h"
#include "../../include/util/imagetools.h"

void Integrator::render(const Scene& scene)
{
  int hRes = 320, vRes = 240;
  ImageTools::RGBuchar *img = new ImageTools::RGBuchar[hRes*vRes];

  for(int i = 0; i < vRes; ++i)
    for(int j = 0; j < hRes; ++j)
    {
      float half_vRes = vRes*0.5f, half_hRes = hRes*0.5f;
      float u = (half_vRes - i)/ half_vRes;
      float v = (j - half_hRes)/ half_hRes;

      RGB sample = integrate(Vec2(u,v), scene);
      img[i*hRes+j] = ImageTools::rgb_float_to_uchar(sample);
    }

  ImageTools::rgb_write_to_file("../output/out.ppm", hRes, vRes, img);
  delete[] img;
}

Integrator::ptr Integrator::load_from_json(const nlohmann::json& in)
{
  return Integrator::ptr( new Pathtracer );
}
