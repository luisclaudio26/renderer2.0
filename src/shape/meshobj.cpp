#include "../../include/shape/meshobj.h"
#include "../../include/material/mtl.h"
#include "../../include/texture/imagetexture.h"
#include "../../include/util/log.h"
#include <glm/gtc/type_ptr.hpp>

static std::string fix_path(const std::string& path)
{
  std::string out;
  for(int i = 0; i < path.length(); ++i)
    if(path[i] == '\\') out += '/';
    else out += path[i];
  return out;
}

//----------------------------------
//--------- FROM MESHOBJ.H ---------
//----------------------------------
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

void MeshOBJ::load_material_data(const std::string& basedir,
                                 const std::vector<tinyobj::material_t>& materials)
{
  //fill this->materials vector with data from material_t vector
  //Order must be the same, so the pointers in the triangles will be valid
  //Also, load images
  //TODO: retrieve texture names, decompress, load and store inside
  //textures vector
  for(auto m = materials.begin(); m != materials.end(); ++m)
  {
    MTL *new_m = new MTL;
    new_m->illum_model = m->illum;
    new_m->emission = glm::make_vec3(m->emission);
    new_m->ambient = glm::make_vec3(m->ambient);
    new_m->diffuse = glm::make_vec3(m->diffuse);
    new_m->specular = glm::make_vec3(m->specular);
    new_m->transmittance = glm::make_vec3(m->transmittance);
    new_m->dissolve = m->dissolve;
    new_m->ior = m->ior;
    new_m->shininess = m->shininess;

    //load texture (if they exist)
    //TODO: this assumes texture is an image, but .obj
    //file supports procedural textures also
    if(!m->ambient_texname.empty())
      set_texture(basedir + m->ambient_texname, new_m->amb_tex);

    if(!m->diffuse_texname.empty())
      set_texture(basedir + m->diffuse_texname, new_m->diff_tex);

    if(!m->specular_texname.empty())
      set_texture(basedir + m->specular_texname, new_m->spec_tex);

    if(!m->bump_texname.empty())
      set_texture(basedir + m->bump_texname, new_m->bump_tex);

    if(!m->displacement_texname.empty())
      set_texture(basedir + m->displacement_texname, new_m->height_tex);

    if(!m->alpha_texname.empty())
      set_texture(basedir + m->alpha_texname, new_m->dissolve_tex);

    this->materials.push_back( Material::ptr(new_m) );
  }

  ERROR("TODO: load_material_data()\n");
}

void MeshOBJ::set_texture(const std::string& path, int& target_id)
{
  ImageTexture *im = new ImageTexture( fix_path(path) );
  this->textures.push_back( Texture::ptr(im) );
  target_id = this->textures.size()-1;
}

std::string MeshOBJ::str() const
{
  std::string out("");
  out += std::to_string(tris.size()) + std::string(" triangles\n");

  for( auto m : materials )
    out += std::string("\t") + m->str() + std::string("\n");

  for( auto t : textures )
    out += std::string("\t") + t->str() + std::string("\n");

  return out;
}
