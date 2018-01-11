#include <fstream>
#include <FreeImage.h>
#include "../3rdparty/json.hpp"
#include "../include/core/sceneloader.h"
#include "../include/core/integrator.h"

int main(int argc, char** args)
{
  srand(0);
  FreeImage_Initialise(false);

  //read input file
  std::fstream in( args[1] );
  nlohmann::json j; in >> j;

  //load data
  SceneLoader *loader = new SceneLoader;
  loader->load_scene_from_json( j );

  Integrator::ptr integrator = Integrator::load_from_json( j );

  //preprocess scene
  Scene *scene = new Scene;
  loader->generate_scene(*scene);

  //hora do show
  integrator->render(*scene);

  //clean up
  delete loader;
  delete scene;
  
  FreeImage_DeInitialise();

  return 0;
}
