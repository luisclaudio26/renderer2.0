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

  if( type.compare("meshOBJ") == 0 )
  {
    tinyobj::attrib_t attrib; std::string err;
    std::vector<tinyobj::material_t> materials;
    std::vector<tinyobj::shape_t> shapes;

    std::string path = param["path"].get<std::string>();
    std::string basedir = basedir_from_path(path).c_str();

    tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                      path.c_str(), basedir.c_str());
    if(!err.empty()) ERROR( err.c_str() );

    //load .obj to internal format
    MeshOBJ* obj = new MeshOBJ; //TODO: memory leak! check this
    obj->load_geometry_data( shapes, attrib );

    //TODO: if material = materialMTL, load it here and than return
    //shape. If not, skip this and do not return, so material will
    //be properly loaded after
    if( mat["type"].get<std::string>().compare("materialMTL") == 0 )
    {
      obj->load_material_data( basedir, materials );
      obj->model2world = model2world;

      return Shape::ptr( obj );
    }
  }
  else ERROR("only meshOBJ shapes are supported!")

  //common attributes
  out->model2world = model2world;
  Material::load_from_json( in["material"], out->materials, out->textures );
}
