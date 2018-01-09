#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "shape.h"
#include "primitive.h"
#include "material.h"
#include "texture.h"
#include "background.h"
#include "scene.h"
#include <vector>
#include "../3rdparty/json.hpp"

class SceneLoader
{
private:
  std::vector<Shape::ptr> shapes;
  Camera camera;
  Background bgd;

  std::vector<Primitive> prims;
  std::vector<Material> materials;
  std::vector<Texture> textures;

public:
  bool load_scene_from_json(const nlohmann::json& in);
  void generate_primitives(Scene& target);
};

#endif
