#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "../core/texture.h"
#include "../core/spectrum.h"
#include <FreeImage.h>

class ImageTexture : public Texture
{
private:
  FIBITMAP* img;

  //I don't know how to use the Alpha channel
  //in a texture, but some .obj files do have
  //transparent textures
  RGBA *data;

  int w, h;

public:
  ImageTexture(const std::string& path);
  ~ImageTexture() override;
};

#endif
