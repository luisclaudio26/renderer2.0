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
  std::vector<std::vector<Primitive::ptr>> unpacked_prims;
  unpacked_prims.resize(shapes.size());

  //generate primitives
  int s_id = 0;
  for( auto s : shapes )
  {
    s->generate_primitives( unpacked_prims[s_id++] );
    printf("generated %d primitives\n", unpacked_prims[s_id-1].size());
  }

  //pack textures
  int tex_offset = 0;
  for( auto s : shapes )
  {
    std::vector<Texture>& texs = s.textures;

    //TODO: this is really inefficient!
    for( auto t : texs ) target.textures.push_back(t);

    //reindex texture pointers inside materials
    for( auto m : s.materials )
    {
      if(m.amb_tex > 0) m.amb_tex += tex_offset;
      if(m.diff_tex > 0) m.diff_tex += tex_offset;
      if(m.spec_tex > 0) m.spec_tex += tex_offset;
      if(m.bump_tex > 0) m.bump_tex += tex_offset;
      if(m.height_tex > 0) m.height_tex += tex_offset;
      if(m.dissolve_tex > 0) m.dissolve_tex += tex_offset;
    }


  }
}
