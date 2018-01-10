#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
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
                              std::vector<Material>& target_mat,
                              std::vector<Texture>& target_tex);
};

#endif
