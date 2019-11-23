#include "TrackerEdController.h"
#include "Project/Project.h"

#include <imgui.h>
#include <GL/glew.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/imgproc/imgproc.hpp>

#include <boost/filesystem.hpp>



TrackerEdController::TrackerEdController() : view(model) {
    this->view.SetAddClipCallback(std::bind(&TrackerEdController::addClipCallback, this, std::placeholders::_1));



    //this->view.SetAddClipCallback(this->addClipCallback)
}

TrackerEdController::~TrackerEdController() {

}


void TrackerEdController::Init() {
    this->view.Init();
}

void TrackerEdController::DeInit() {
    this->view.DeInit();
}

void TrackerEdController::Render() {
    this->view.Render();
}


void TrackerEdController::addClipCallback(std::string filePath) {
    Project::GetInstance().ImportFile(filePath);
    this->cacheVideo(filePath);
}


void TrackerEdController::cacheVideo(std::string filePath) {
    cv::VideoCapture video(filePath);

    cv::Mat videoFrame;
    GLuint videoFrameGlTexture = 0;
    while (video.read(videoFrame)) {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenTextures(1, &videoFrameGlTexture);
        glBindTexture(GL_TEXTURE_2D, videoFrameGlTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        cv::cvtColor(videoFrame, videoFrame, cv::COLOR_RGB2BGR);

        glTexImage2D(GL_TEXTURE_2D,         // Type of texture
            0,                   // Pyramid level (for mip-mapping) - 0 is the top level
            GL_RGB,              // Internal colour format to convert to
            videoFrame.cols,          // Image width  i.e. 640 for Kinect in standard mode
            videoFrame.rows,          // Image height i.e. 480 for Kinect in standard mode
            0,                   // Border width in pixels (can either be 1 or 0)
            GL_RGB,              // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
            GL_UNSIGNED_BYTE,    // Image data type
            videoFrame.ptr());        // The actual image data itself

        this->model.ClipTextureCollection.push_back((ImTextureID)(intptr_t)videoFrameGlTexture);
    }

}



void TrackerEdController::detectORB(int frameIdx) {

}
