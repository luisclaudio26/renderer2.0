#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <string>

class Texture
{
private:
public:
  typedef std::shared_ptr<Texture> ptr;

  virtual std::string str() const = 0;
  virtual ~Texture() {}
};

#endif
