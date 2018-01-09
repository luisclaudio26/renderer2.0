#include <fstream>
#include "../3rdparty/json.hpp"
#include "../include/core/sceneloader.h"

int main(int argc, char** args)
{
  srand(0);

  //read input file
  std::fstream in( args[1] );
  nlohmann::json j; in >> j;

  //load data
  SceneLoader loader;
  loader.load_scene_from_json( json );

  //preprocess scene
  Scene scene;
  loader.generate_scene(scene);

  //hora do show
  scene.render();

  return 0;
}
