#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <vector>
#include <string>
#include "geometry.h"
#include "material.h"
#include "texture.h"
#include "primitive.h"
#include "../../3rdparty/json.hpp"

class Shape
{
public:
  typedef std::shared_ptr<Shape> ptr;

  //a single shape may have many materials, and a
  //single material can have use many texture channels.
  //primitives generated with generate_primitives() have their
  //material/texture pointers indexing elements in these two
  //vectors
  std::vector<Material::ptr> materials;
  std::vector<Texture::ptr> textures;

  Mat4 model2world;

  //this method won't erase any contents that TARGET may eventually have
  virtual void generate_primitives(std::vector<Primitive::ptr>& target) const = 0;

  static Shape::ptr load_from_json(const nlohmann::json& in);
  virtual std::string str() const = 0;
};

#endif
