#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

//TODO: gpurenderer's materials are represented as a union
//here we're gonna use polymorphism, so one of the materials
//will be MTLmaterial, for example. I still don't know the
//virtual methods
class Material
{
private:
public:
  typedef std::shared_ptr<Material> ptr;
};

#endif
