#include "../../include/core/sceneloader.h"

bool SceneLoader::load_scene_from_json(const nlohmann::json& in)
{
  this->camera = Camera::load_from_json( in["camera"] );
  this->bgd = Background::load_from_json( in["background"] );

  nlohmann::json shapes = in["geometry"];
  for( auto shape : shapes )
    this->shapes.push_back( Shape::load_from_json(shape) );

  return true;
}

void SceneLoader::generate_scene(Scene& target)
{

}
