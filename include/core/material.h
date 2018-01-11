#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <string>
#include "../../3rdparty/json.hpp"
#include "texture.h"

//TODO: gpurenderer's materials are represented as a union
//here we're gonna use polymorphism, so one of the materials
//will be MTLmaterial, for example. I still don't know the
//virtual methods
class Material
{
public:
  typedef std::shared_ptr<Material> ptr;

  //collection of BRDF/BTDF (BSDF)
  //textures modulating the BSDF

  //load material data from JSON file into material/texture vectors.
  //in case we need more than one material to be loaded (in the case of
  //.mtl files, for example, where many different materials are described),
  //we take target vectors
  static void load_from_json(const nlohmann::json& in,
                              std::vector<Material::ptr>& target_mat,
                              std::vector<Texture::ptr>& target_tex);

  //packing textures into a single vector requires
  //texture indices inside material to be reorganized
  virtual void reindex_textures(int offset) = 0;

  virtual std::string str() const = 0;
};

#endif
