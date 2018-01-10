#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <memory>
#include "geometry.h"
#include "scene.h"
#include "spectrum.h"
#include "../../3rdparty/json.hpp"

class Integrator
{
private:
  //given a pixel in screen space ( [-1,1]² ) and scene data, compute its
  //luminance. this function is responsible for tracing primary rays!
  virtual RGB integrate(const Vec2& uv, const Scene& scene) const = 0;

public:
  typedef std::shared_ptr<Integrator> ptr;

  void render(const Scene& scene);
  static Integrator::ptr load_from_json(const nlohmann::json& in);
};

#endif
