#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "shape.h"
#include "primitive.h"
#include "material.h"
#include "texture.h"
#include "background.h"
#include "camera.h"
#include "scene.h"
#include <vector>
#include "../3rdparty/json.hpp"

class SceneLoader
{
private:
  std::vector<Shape::ptr> shapes;
  Camera::ptr camera;
  Background::ptr bgd;

  std::vector<Primitive::ptr> prims;
  std::vector<Material::ptr> materials;
  std::vector<Texture::ptr> textures;

public:
  bool load_scene_from_json(const nlohmann::json& in);
  void generate_scene(Scene& target);
};

#endif
