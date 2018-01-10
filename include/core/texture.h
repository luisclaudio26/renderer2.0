#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

class Texture
{
private:
public:
  typedef std::shared_ptr<Texture> ptr;

  virtual ~Texture() {}
};

#endif
