#ifndef GUI_H
#define GUI_H

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>

class GUI : public nanogui::Screen
{
private:
  nanogui::GLShader shader;

public:
  GUI();
  virtual void drawContents();
  virtual void draw(NVGcontext *ctx);
};

#endif
