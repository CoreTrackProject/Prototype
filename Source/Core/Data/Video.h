#pragma once

#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <imgui.h>



#include "Task/Video/ReadVideo.h"
#include <GL/glew.h>

#include <libmv/autotrack/region.h>
#include <libmv/autotrack/frame_accessor.h>


#include <boost/log/trivial.hpp>

#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

class Video {

private:
    std::string videoPath;

	cv::Size videoDim;

    // Cursor from where to cache
    int cursorPosition = 0;

    int frameCount = 0;

    int startFrame, endFrame;

    GLuint videoFrameTexture;

	cv::VideoCapture videoCapture;

private:

    void initVideoFrameTexture();

public:
    Video();

    Video(std::string filePath);

    ~Video();
	
public:
    std::string GetVideoPath();

    void ImportVideoFromPath(std::string filePath);

    cv::Mat GetFrameByIdx(int idx);

    ImTextureID GetTextureFrameByIdx(int idx);

    bool HasFrames();	

    int GetFrameCount();

	cv::Size GetVideoDim();
};