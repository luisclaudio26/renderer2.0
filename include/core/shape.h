#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <vector>
#include "geometry.h"
#include "material.h"
#include "texture.h"
#include "primitive.h"

class Shape
{
private:
  Mat4 model2world;

  //a single shape may have many materials, and a
  //single material can have use many texture channels.
  //primitives generated with generate_primitives() have their
  //material/texture pointers indexing elements in these two
  //vectors
  std::vector<Material> materials;
  std::vector<Texture> textures;

public:
  typedef std::shared_ptr<Shape> ptr;

  //this method won't erase any contents that TARGET may eventually have
  virtual void generate_primitives(std::vector<Primitive>& target) const = 0;
};

#endif
