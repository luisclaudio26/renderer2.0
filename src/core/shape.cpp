#include "../../include/core/shape.h"
#include "../../include/shape/meshobj.h"
#include "../../include/util/jsonhelper.h"
#include "../../include/util/log.h"
#include "../../3rdparty/tiny_obj_loader.h"
#include <string>

std::string basedir_from_path(const std::string& path)
{
  std::size_t found = path.find_last_of('/');
  return path.substr(0, found+1);
}

//----------------------------
//------- FROM SHAPE.H -------
//----------------------------
Shape::ptr Shape::load_from_json(const nlohmann::json& in)
{
  std::string type = in["shape"].get<std::string>();
  Mat4 model2world = JSONHelper::load_transformation( in["transformation"] );
  nlohmann::json param = in["shapeParam"];
  nlohmann::json mat = in["material"];

  Shape::ptr out;

  //materials must be loaded FIRST, as primitives will need to reference them
  //Both of these vectors will move-assigned after, so we won't do wasteful
  //copies
  std::vector<Material::ptr> materials_tmp;
  std::vector<Texture::ptr> textures_tmp;
  Material::load_from_json( in["material"], materials_tmp, textures_tmp );

  //now load the actual shape
  if( type.compare("meshOBJ") == 0 )
  {
    tinyobj::attrib_t attrib; std::string err;
    std::vector<tinyobj::material_t> materials;
    std::vector<tinyobj::shape_t> shapes;

    std::string path = param["path"].get<std::string>();
    std::string basedir = basedir_from_path(path).c_str();

    //load data from .obj file. unfortunatelly
    //this will force the materials to be reloaded, but
    //that's not much of a problem
    tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                      path.c_str(), basedir.c_str());
    if(!err.empty()) ERROR( err.c_str() );

    //transform tinyobj object into internal format
    MeshOBJ* obj = new MeshOBJ; //TODO: memory leak! check this

    //we need materials before loading geometry
    obj->textures = std::move(textures_tmp);
    obj->materials = std::move(materials_tmp);
    obj->load_geometry_data( shapes, attrib );

    //at this point we have nothing more to do and can return
    obj->model2world = model2world;

    return Shape::ptr(obj);
  }
  else ERROR("only meshOBJ shapes are supported!")

  //common attributes
  out->model2world = model2world;
  out->textures = std::move(textures_tmp);
  out->materials = std::move(materials_tmp);
}
