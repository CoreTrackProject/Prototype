#pragma once

#include <string>
#include <imgui.h>
#include <opencv2/core.hpp>
#include <GL/glew.h>

class VideoUtils {

public:
    static int GetVideoFrameCount(std::string filePath);

    static GLuint MatToGLuint(cv::Mat frame);
};
