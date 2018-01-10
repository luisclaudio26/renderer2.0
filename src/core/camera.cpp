#include "../../include/core/camera.h"
#include "../../include/camera/pinhole.h"
#include "../../include/util/jsonhelper.h"

Camera::ptr Camera::load_from_json(const nlohmann::json& in)
{
  //TODO: in the future, we shall create different cameras
  //(ortographics, projective) depending on the parameters.
  Vec3 pos = JSONHelper::load_vec3(in["pos"]);
  Vec3 up = JSONHelper::load_vec3( in["up"] );
  Vec3 look = JSONHelper::load_vec3( in["look_at"] );
  float fw = in["film_width"].get<float>();
  float ar = in["aspect_ratio"].get<float>();
  float d = in["focal_length"].get<float>();
  int vRes = in["vRes"].get<int>();
  int hRes = in["hRes"].get<int>();

  return Camera::ptr( new Pinhole(pos, up, look, fw, ar, d, hRes, vRes) );
}
