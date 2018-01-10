#ifndef SOLID_H
#define SOLID_H

#include "../core/background.h"

class Solid : public Background
{
private:
  RGB color;

public:
  Solid(const RGB& color) : color(color) {}

  RGB sample(const Ray& r) const override { return this->color; }
};

#endif
