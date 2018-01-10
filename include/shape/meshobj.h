#ifndef MESHOBJ_H
#define MESHOBJ_H

#include "../../3rdparty/tiny_obj_loader.h"
#include "../core/shape.h"
#include "../primitive/triangle.h"

class MeshOBJ : public Shape
{
private:
  //loads texture and return its ID inside the textures vector
  void set_texture(const std::string& path, int& target_id);

public:
  std::vector<Triangle> tris;

  void load_geometry_data(const std::vector<tinyobj::shape_t>& shapes,
                          const tinyobj::attrib_t& attrib);

  void load_material_data(const std::string& basedir,
                          const std::vector<tinyobj::material_t>& materials);

  void generate_primitives(std::vector<Primitive>& target) const override;

  std::string str() const override;
};

#endif
