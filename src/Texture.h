#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <string_view>

class Texture
{
public:
    Texture(unsigned char* data, int width, int height);
    GLuint getID() const;

private:
    GLboolean IsExtensionSupported(std::string_view name) const;

    GLuint ID = 0;
};

#endif