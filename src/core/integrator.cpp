#include "../../include/core/integrator.h"
#include "../../include/integrator/pathtracer.h"
#include <thread>
#include <cstdio>
#include <chrono>
using namespace std::chrono;

void Integrator::render_patch(ImageTools::RGBuchar* img, const Scene& scene,
                              int row, int col, int height, int width,
                              int vRes, int hRes, int thread_id)
{
  //this function is meant to be executed by an specific thread
  //which will render this block of WidthxHeight size, starting
  //at (row, col)
  double acc_time = 0.0f; int pixel_count = 0;
  int n_pixels = height*width;

  for(int i = row; i < row+height; ++i)
    for(int j = col; j < col+width; ++j)
    {
      float half_vRes = vRes*0.5f, half_hRes = hRes*0.5f;

      float u = (j - half_hRes)/ half_hRes;
      float v = (half_vRes - i)/ half_vRes;

      high_resolution_clock::time_point tS = high_resolution_clock::now();
      RGB sample = integrate(Vec2(u,v), scene);
      high_resolution_clock::time_point tE = high_resolution_clock::now();

      duration<double> time_span = duration_cast<duration<double>>(tE - tS);
      acc_time += time_span.count();

      img[i*hRes+j] = ImageTools::rgb_float_to_uchar(sample);

      pixel_count++;


      if(pixel_count % 100 == 0)
        printf("(Thread %d) %f%%\n", thread_id, (float)100.0f*pixel_count / n_pixels);
    }

  printf("Avg integration time (thread %d): %fs\n", thread_id,
                                                    acc_time/n_pixels);
}

void Integrator::render(const Scene& scene)
{
  int hRes = scene.cam->hRes, vRes = scene.cam->vRes;
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
                                              img, scene, row, col, height, width,
                                              vRes, hRes, (i*hori_sectors)+j) );
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
