#include "../include/frontend/gui.h"
#include <nanogui/button.h>
#include <nanogui/layout.h>
#include <iostream>

#include "../3rdparty/json.hpp"
#include "../include/core/sceneloader.h"
#include "../include/core/integrator.h"
#include <fstream>

GUI::GUI() : nanogui::Screen(Eigen::Vector2i(100, 100), "Andaluz renderer", false)
{
  using namespace nanogui;

  // TODO: disable window resizeability so we won't have
  // to deal with buffer reallocation
  Window* window = new Window(this, "Options");
  window->setPosition(Vector2i(0, 0));
  window->setLayout(new GroupLayout());

  performLayout();

  // TODO: initFromFiles is looking for path relative to the
  // first .cpp that is including gui.h. Report to github?!
  shader.initFromFiles("passthrough",
                        "../include/frontend/passthrough.vs",
                        "../include/frontend/passthrough.fs");

  // upload the triangles we'll use to draw onto
  Eigen::MatrixXf quad(2, 6);
  quad.col(0)<<-1.0f, -1.0f; //lower triangle
  quad.col(1)<<+1.0f, -1.0f;
  quad.col(2)<<+1.0f, +1.0f;
  quad.col(3)<<-1.0f, -1.0f; //upper triangle
  quad.col(4)<<+1.0f, +1.0f;
  quad.col(5)<<-1.0f, +1.0f;

  Eigen::MatrixXf uv(2, 6);
  uv.col(0)<<0.0f, 1.0f; //lower triangle
  uv.col(1)<<1.0f, 1.0f;
  uv.col(2)<<1.0f, 0.0f;
  uv.col(3)<<0.0f, 1.0f; //upper triangle
  uv.col(4)<<1.0f, 0.0f;
  uv.col(5)<<0.0f, 0.0f;

  shader.bind();
  shader.uploadAttrib<Eigen::MatrixXf>("pos", quad);
  shader.uploadAttrib<Eigen::MatrixXf>("uv_vertex", uv);

  // --------- initialize renderer -----------
  //read input file
  std::fstream in( "../data/cornellbox.json" );
  nlohmann::json j; in >> j;

  //load data
  SceneLoader loader;
  loader.load_scene_from_json( j );

  this->integrator = Integrator::load_from_json( j );

  //preprocess scene
  loader.generate_scene(this->scene);
  scene.preprocess();

  // --------- set GUI parameters ----------
  //resize according to scene
  this->setSize( Eigen::Vector2i(scene.cam->hRes, scene.cam->vRes) );

  // initialize color_buffer in CPU
  int n_pixels = this->width() * this->height();
  color_buffer_cpu.resize(n_pixels);

  for(Eigen::Vector4f &p : color_buffer_cpu)
    p<<0.1f, 0.1f, 0.1f, 1.0f;

  // initialize texture which will hold image
  glGenTextures(1, &color_buffer_gpu);
  glBindTexture(GL_TEXTURE_2D, color_buffer_gpu);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->width(), this->height());
};

void GUI::drawContents()
{
  //---------- request samples ---------
  std::vector<float> samples;
  integrator->render(scene, samples, this->width(), this->height());
  this->push_samples(samples);

  //---------- displaying ----------
  shader.bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_buffer_gpu);

  //TODO: SegFault comes from this call! Problem with color_buffer_cpu??
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width(), this->height(),
  	               GL_RGBA, GL_FLOAT, (void*)color_buffer_cpu.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  shader.setUniform("color_buffer", 0);
  shader.drawArray(GL_TRIANGLES, 0, 6);
}

void GUI::draw(NVGcontext *ctx) { Screen::draw(ctx); }

void GUI::push_samples(std::vector<float>& pixel_data)
{
  static int n_samples = 0;

  n_samples++;

  RGBA *rgba_data = reinterpret_cast<RGBA*>(pixel_data.data());

  // to compute the average:
  // (a1 + a2 + ... + an) / n
  // avg * n + (an+1) / (n+1)

  for(int i = 0; i < pixel_data.size(); i += 4)
  {
    RGBA new_sample;
    new_sample<<pixel_data[i], pixel_data[i+1], pixel_data[i+2], pixel_data[i+3];

    RGBA &p = color_buffer_cpu[i/4];
    p = ( (n_samples-1.0f) * p + new_sample) / n_samples;
  }


  //color_buffer_cpu = std::vector<RGBA>(rgba_data, rgba_data + pixel_data.size() / 4);
}
