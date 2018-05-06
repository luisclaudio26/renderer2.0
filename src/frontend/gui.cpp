#include "../include/frontend/gui.h"

GUI::GUI() : nanogui::Screen(Eigen::Vector2i(960, 540), "Andaluz renderer")
{
  using namespace nanogui;

  // TODO: disable window resizeability so we won't have
  // to deal with buffer reallocation

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
  shader.bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_buffer_gpu);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width(), this->height(),
  	               GL_RGBA, GL_FLOAT, (void*)color_buffer_cpu.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  shader.setUniform("color_buffer", 0);
  shader.drawArray(GL_TRIANGLES, 0, 6);
}

void GUI::draw(NVGcontext *ctx) { Screen::draw(ctx); }
