#pragma once

#include <string>
#include <imgui.h>
#include <opencv2/core.hpp>
#include <GL/glew.h>

#include <libmv/image/image.h>

class VideoUtils {

public:
    static int GetVideoFrameCount(std::string filePath);

    static GLuint MatToGLuint(cv::Mat frame);

    static libmv::FloatImage MatToFloatImage(cv::Mat &image);

};
