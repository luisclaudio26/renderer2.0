#include "../../include/core/integrator.h"
#include "../../include/integrator/pathtracer.h"
#include <thread>
#include <cstdio>

void Integrator::render_patch(ImageTools::RGBuchar* img, const Scene& scene,
                              int row, int col, int height, int width,
                              int vRes, int hRes)
{
  //this function is meant to be executed by an specific thread
  //which will render this block of WidthxHeight size, starting
  //at (row, col)
  //TODO: some bug is causing the image to be flipped, check this after!
  for(int i = row; i < row+height; ++i)
    for(int j = col; j < col+width; ++j)
    {
      float half_vRes = vRes*0.5f, half_hRes = hRes*0.5f;

      float u = (j - half_hRes)/ half_hRes;
      float v = (half_vRes - i)/ half_vRes;

      RGB sample = integrate(Vec2(u,v), scene);
      img[i*hRes+j] = ImageTools::rgb_float_to_uchar(sample);
    }
}

void Integrator::render(const Scene& scene)
{
  int hRes = 640, vRes = 480;
  ImageTools::RGBuchar *img = new ImageTools::RGBuchar[hRes*vRes];

  //TODO: general code for spawning threads:
  //-> find the least p,q such that p.q = N and divide in PxQ sectors
  //adjustments are needed because this will fail if image dimension is
  //not multiple of p and q!!!
  std::vector<std::thread> rendering_threads;
  int vert_sectors = 2, hori_sectors = 4;

  for(int i = 0; i < vert_sectors; ++i)
    for(int j = 0; j < hori_sectors; ++j)
    {
      int height = vRes / vert_sectors;
      int width = hRes / hori_sectors;
      int row = i * height;
      int col = j * width;

      rendering_threads.push_back(std::thread(&Integrator::render_patch, this,
                                              img, scene, row, col,
                                              height, width, vRes, hRes));
    }

  for(auto t = rendering_threads.begin(); t != rendering_threads.end(); ++t)
    t->join();

  ImageTools::rgb_write_to_file("../output/out.ppm", hRes, vRes, img);
  delete[] img;
}

Integrator::ptr Integrator::load_from_json(const nlohmann::json& in)
{
  return Integrator::ptr( new Pathtracer );
}
