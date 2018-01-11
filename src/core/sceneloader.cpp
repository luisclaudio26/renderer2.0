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
  //TODO: after fixing everything converning material/texture
  //loading, this method will become way more simple; we'll
  //simply loop over all the shapes and copy the pointers to
  //a single vector. In the end, materials/textures will be
  //pointer by pointers inside the primitives AND inside the
  //vectors. We need this to correctly deallocate the resources
  //(or maybe not, as shared_ptr's will do this automatically)
  //and maybe apply some operation to all materials/textures
  //at once
  std::vector<std::vector<Primitive::ptr>> unpacked_prims;
  unpacked_prims.resize(shapes.size());

  //generate primitives
  int s_id = 0;
  for( auto s : shapes )
  {
    s->generate_primitives( unpacked_prims[s_id++] );
    printf("generated %d primitives\n", unpacked_prims[s_id-1].size());
  }

  //pack textures and materials into a single vector
  //this isn't really needed for the operations, but will
  //help in managing
  int tex_offset = 0;
  for( auto s : shapes )
  {
    std::vector<Texture::ptr>& texs = s->textures;

    //TODO: this is really inefficient!
    for( auto t : texs ) target.textures.push_back(t);
  }

}
