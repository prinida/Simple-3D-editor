#include "Texture.h"

#include <string_view>

Texture::Texture(unsigned char* data, int width, int height)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (IsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
    {
        GLfloat anisoSetting = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisoSetting);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisoSetting);
    }
}

GLuint Texture::getID() const
{
    return ID;
}

GLboolean Texture::IsExtensionSupported(std::string_view name) const
{
    GLint n = 0;
    const char* extension;

    glGetIntegerv(GL_NUM_EXTENSIONS, &n);

    for (int i = 0; i < n; i++)
    {
        extension = (const char*)glGetStringi(GL_EXTENSIONS, i);

        if (!strcmp(name.data(), extension))
            return GL_TRUE;
    }

    return GL_FALSE;
}
