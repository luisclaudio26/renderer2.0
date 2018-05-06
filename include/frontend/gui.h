#ifndef GUI_H
#define GUI_H

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <vector>

class GUI : public nanogui::Screen
{
private:
  typedef Eigen::Vector4f RGBA;

  nanogui::GLShader shader;

  std::vector<RGBA> color_buffer_cpu;
  GLuint color_buffer_gpu;
public:
  GUI();
  virtual void drawContents();
  virtual void draw(NVGcontext *ctx);
};

#endif
