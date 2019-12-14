#pragma once

#include <string>

#include <opencv2/core.hpp>
#include <imgui.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}

#include "Task/Video/ReadVideo.h"
#include <GL/glew.h>

struct VideoFrame {

    bool IsCached = false;

    //GLuint FrameAsTexture;

    cv::Mat Frame;

    void Clear() {
        this->IsCached = false;
        this->Frame.release();
        //glDeleteTextures(1, &this->FrameAsTexture);
    }

};

class Video {

private:
    std::string videoPath;

    // Cache size in gigabyte
    int maxCacheSizeGB = 2;


    // Cursor from where to cache
    int cursorPosition = 0;

    int frameCount = 0;

    std::vector<VideoFrame> videoFrameCollection;

    ReadVideo videoReaderTask;

    int startFrame, endFrame;

    GLuint videoFrameTexture;

private:

    void updateFrameCollection(int startFrame, int endFrame);
    void emptyCache();
    void initVideoFrameTexture();

public:
    Video();

    Video(std::string filePath);

    ~Video();


public:
    std::string GetVideoPath();

    void ImportVideoFromPath(std::string filePath);

    cv::Mat GetFrameByIdx(int idx, bool cacheForward = false);

    ImTextureID GetTextureFrameByIdx(int idx, bool cacheForward = false);

    bool HasFrames();

    int GetFrameCount();

};
