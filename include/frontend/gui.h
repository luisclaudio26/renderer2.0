#ifndef GUI_H
#define GUI_H

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <vector>

#include "../core/sceneloader.h"
#include "../core/integrator.h"

class GUI : public nanogui::Screen
{
private:
  typedef Eigen::Vector4f RGBA;

  nanogui::GLShader shader;

  Integrator::ptr integrator;
  Scene scene;

  //TODO: lock access to color_buffer_cpu, because the
  //GUI thread should not read from color_buffer_cpu
  //while the rendering thread is writing samples to it
  std::vector<RGBA> color_buffer_cpu;
  GLuint color_buffer_gpu;
public:
  GUI();
  virtual void drawContents();
  virtual void draw(NVGcontext *ctx);

  void push_samples(std::vector<float>& pixel_data);
};

#endif
