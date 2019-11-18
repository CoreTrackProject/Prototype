#pragma once

#include <GL/gl3w.h>

#include <string>

class File {

public:
    // From example: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
    static bool PickSingleFile(std::string &out_filePath);

    static std::string CalcFileHash(std::string filePath);

};
