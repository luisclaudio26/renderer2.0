#include "../../include/core/sceneloader.h"

bool SceneLoader::load_scene_from_json(const nlohmann::json& in)
{
  this->camera = Camera::load_from_json( in["camera"] );
  this->bgd = Background::load_from_json( in["background"] );

  nlohmann::json shapes = in["geometry"];
  for( auto shape : shapes )
    this->shapes.push_back( Shape::load_from_json(shape) );

  //log stuff
  printf("-----------------------\n");
  printf("Camera -> %s\n", this->camera->str().c_str());
  printf("Background -> %s\n", this->bgd->str().c_str());
  for( auto s : this->shapes ) printf("Shape -> %s\n", s->str().c_str());
  printf("-----------------------\n");

  return true;
}

void SceneLoader::generate_scene(Scene& target)
{

}
