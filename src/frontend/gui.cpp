#include "../include/frontend/gui.h"

GUI::GUI() : nanogui::Screen(Eigen::Vector2i(960, 540), "Andaluz renderer")
{
  using namespace nanogui;

  performLayout();

  // TODO: initFromFiles is looking for path relative to the
  // first .cpp that is including gui.h. Report to github?!
  shader.initFromFiles("passthrough",
                        "../include/frontend/passthrough.vs",
                        "../include/frontend/passthrough.fs");


  //these coordinates are, at the same time, positions
  //for the vertices of the quad AND coordinate textures
  Eigen::MatrixXf quad_uv(2, 6);
  quad_uv.col(0)<<-1.0f, -1.0f; //lower triangle
  quad_uv.col(1)<<+1.0f, -1.0f;
  quad_uv.col(2)<<+1.0f, +1.0f;
  quad_uv.col(3)<<-1.0f, -1.0f; //upper triangle
  quad_uv.col(4)<<+1.0f, +1.0f;
  quad_uv.col(5)<<-1.0f, +1.0f;

  shader.bind();
  shader.uploadAttrib<Eigen::MatrixXf>("pos", quad_uv);
  shader.uploadAttrib<Eigen::MatrixXf>("uv_vertex", quad_uv);
};

void GUI::drawContents()
{
  shader.bind();
  shader.setUniform("color", Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
  shader.drawArray(GL_TRIANGLES, 0, 6);
}

void GUI::draw(NVGcontext *ctx) { Screen::draw(ctx); }
