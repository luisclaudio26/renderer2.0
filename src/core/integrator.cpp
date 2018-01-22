#include "../../include/core/integrator.h"
#include "../../include/integrator/pathtracer.h"
#include <thread>
#include <mutex>
#include <cstdio>
#include <chrono>
using namespace std::chrono;

#define SPP 128
static std::mutex mtx;
static int n_threads;
static float* progress_bar;

//-----------------------------------------
//---------- FROM INTEGRATOR.H ------------
//-----------------------------------------
void Integrator::render_patch(ImageTools::RGBuchar* img, const Scene& scene,
                              int row, int col, int height, int width,
                              int vRes, int hRes, int thread_id)
{
  //this function is meant to be executed by an specific thread
  //which will render this block of WidthxHeight size, starting
  //at (row, col)
  int n_pixels = height*width;

  #ifdef DEBUG
  double acc_time = 0.0f; int pixel_count = 0;
  #endif

  for(int i = row; i < row+height; ++i)
    for(int j = col; j < col+width; ++j)
    {
      float half_vRes = vRes*0.5f, half_hRes = hRes*0.5f;

      float u = (j - half_hRes)/ half_hRes;
      float v = (half_vRes - i)/ half_vRes;
      float pixel_w = 1.0f / hRes, pixel_h = 1.0f / vRes;

      #ifdef DEBUG
      high_resolution_clock::time_point tS = high_resolution_clock::now();
      #endif

      RGB sample_acc(0.f);
      for(int s = 0; s < SPP; ++s)
      {
        //uniform sampling within pixel area
        float p = (float)rand()/RAND_MAX;
        float q = (float)rand()/RAND_MAX;

        float u_ = u + p*pixel_w;
        float v_ = v + q*pixel_h;

        sample_acc += integrate(Vec2(u_,v_), scene);
      }
      RGB sample = sample_acc / (float)SPP;

      #ifdef DEBUG
      high_resolution_clock::time_point tE = high_resolution_clock::now();

      duration<double> time_span = duration_cast<duration<double>>(tE - tS);
      acc_time += time_span.count();

      pixel_count++;
      if(pixel_count % 500 == 0)
      {
        mtx.lock();
        progress_bar[thread_id] = (float)100.0f*pixel_count/n_pixels;

        printf("\r[");
        for(int i = 0; i < n_threads; ++i) printf("%f ", progress_bar[i]);
        printf("]");
        mtx.unlock();
      }
      #endif

      img[i*hRes+j] = ImageTools::rgb_float_to_uchar(sample);
    }

  #ifdef DEBUG
  printf("Avg integration time (thread %d): %fs\n", thread_id,
                                                    acc_time/n_pixels);
  #endif
}

void Integrator::render(const Scene& scene)
{
  int hRes = scene.cam->hRes, vRes = scene.cam->vRes;
  ImageTools::RGBuchar *img = new ImageTools::RGBuchar[hRes*vRes];

  //TODO: general code for spawning threads:
  //-> find the least p,q such that p.q = N and divide in PxQ sectors
  //adjustments are needed because this will fail if image dimension is
  //not multiple of p and q!!!
  int vert_sectors = 2, hori_sectors = 4;

  #ifdef DEBUG
  n_threads = vert_sectors * hori_sectors;
  progress_bar = new float[n_threads];
  for(int i = 0; i < n_threads; ++i)
    progress_bar[i] = 0.0f;
  #endif

  std::vector<std::thread> rendering_threads;
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
