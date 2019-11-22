#pragma once

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Image {

public:
    // From example: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
    static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

};
