#include <fstream>
#include "../3rdparty/json.hpp"
#include "../include/core/sceneloader.h"
#include "../include/core/integrator.h"

int main(int argc, char** args)
{
  srand(0);

  //read input file
  std::fstream in( args[1] );
  nlohmann::json j; in >> j;

  //load data
  SceneLoader loader;
  loader.load_scene_from_json( j );

  Integrator::ptr integrator = Integrator::load_from_json( j );

  //preprocess scene
  Scene scene;
  loader.generate_scene(scene);

  //hora do show
  integrator->render(scene);

  return 0;
}
