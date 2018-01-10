#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "../core/texture.h"

class ImageTexture : public Texture
{
private:
  float *data;

public:
  ImageTexture(const std::string& path);
};

#endif
