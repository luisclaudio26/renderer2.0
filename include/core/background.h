#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "spectrum.h"
#include <memory>

class Background
{
private:
public:
  typedef std::shared_ptr<Background> ptr;
  virtual RGB sample(const Ray& r) const = 0;
};

#endif
