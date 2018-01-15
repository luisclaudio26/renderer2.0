#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include <string>
#include "geometry.h"
#include "../../3rdparty/json.hpp"

class Camera
{
public:
  typedef std::shared_ptr<Camera> ptr;

  //horizontal and vertical resolution in pixels
  int vRes, hRes;

	virtual Ray getRay(const Vec2& uv) const = 0;

  static Camera::ptr load_from_json(const nlohmann::json& in);
  virtual std::string str() const = 0;
};

#endif
