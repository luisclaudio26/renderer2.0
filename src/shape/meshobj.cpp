#include "../../include/shape/meshobj.h"
#include "../../include/util/log.h"

void MeshOBJ::generate_primitives(std::vector<Primitive>& target) const
{

}

void MeshOBJ::load_geometry_data(const std::vector<tinyobj::shape_t>& shapes,
                                  const tinyobj::attrib_t& attrib)
{
  this->tris.clear();

  //load triangles for each shape
  for(auto s = shapes.begin(); s != shapes.end(); ++s)
  {
    unsigned int attrib_offset = 0;

    for(int f_id = 0; f_id < s->mesh.num_face_vertices.size(); ++f_id)
    {
      int f = s->mesh.num_face_vertices[f_id];
      Triangle face;

      //load vertices for this triangle
      //TODO: WILL FAIL IF MESH IS NOT MADE OF TRIANGLES!
      for(int v_id = 0; v_id < f; v_id++)
      {
        tinyobj::index_t v = s->mesh.indices[attrib_offset + v_id];

        float vx = attrib.vertices[3*v.vertex_index + 0];
        float vy = attrib.vertices[3*v.vertex_index + 1];
        float vz = attrib.vertices[3*v.vertex_index + 2];

        face.v[v_id] = Vec3( vx, vy, vz );
      }

      //load pointer to material. all shapes share the same materials!
      face.material = s->mesh.material_ids[f_id];

      this->tris.push_back( face );
      attrib_offset += f;
    }
  }
}

void MeshOBJ::load_material_data(const std::vector<tinyobj::material_t>& materials)
{

}
