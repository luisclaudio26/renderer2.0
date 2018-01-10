#ifndef MTL_H
#define MTL_H

#include "../core/material.h"
#include "../core/spectrum.h"

//Nothing special; this is just a material
//which properly implements the materials
//described in .mtl files using nice BxDFs
class MTL : public Material
{
public:
  int illum_model;

  RGB emission;

  //each component has an associated
  //texture ID, which is used to modulate it
  RGB ambient; int amb_tex;
  RGB diffuse; int diff_tex;
  RGB specular; int spec_tex; float shininess;
  RGB transmittance; float ior; // index of refraction

  int bump_tex, height_tex;

  // 1 == opaque; 0 == fully transparent
  float dissolve; int dissolve_tex;
};

#endif
