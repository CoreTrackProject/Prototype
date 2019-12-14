#include "Video.h"


#include "Utils/VideoUtils.h"

#include <cassert>


Video::Video() {


}


Video::Video(std::string filePath) {
    this->initVideoFrameTexture();

    this->ImportVideoFromPath(filePath);
}

Video::~Video() {
    glDeleteTextures(1, &this->videoFrameTexture);
}


std::string Video::GetVideoPath() {
    return this->videoPath;
}

void Video::ImportVideoFromPath(std::string filePath) {
    this->videoPath = filePath;
    videoReaderTask.SetVideoFilePath(this->videoPath);

    this->frameCount = VideoUtils::GetVideoFrameCount(filePath);

    this->videoFrameCollection.resize(this->frameCount);

    this->updateFrameCollection(0, 99);

}

cv::Mat Video::GetFrameByIdx(int idx, bool cacheForward) {
    if(!this->videoFrameCollection[idx].IsCached) {
        if(cacheForward) {
            this->updateFrameCollection(idx, idx + 100);
        } else {
            this->updateFrameCollection(idx - 50, idx + 50);
        }
    }
    return this->videoFrameCollection[idx].Frame;
}

ImTextureID Video::GetTextureFrameByIdx(int idx, bool cacheForward) {

    // Update/Convert texture

    if(!this->videoFrameCollection[idx].IsCached) {
        if(cacheForward) {
            this->updateFrameCollection(idx, idx + 100);
        } else {
            this->updateFrameCollection(idx - 50, idx + 50);
        }
    }


    glBindTexture(GL_TEXTURE_2D, this->videoFrameTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
                 0,                // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,           // Internal colour format to convert to
                 this->videoFrameCollection[idx].Frame.cols,       // Image width  i.e. 640 for Kinect in standard mode
                 this->videoFrameCollection[idx].Frame.rows,       // Image height i.e. 480 for Kinect in standard mode
                 0,                // Border width in pixels (can either be 1 or 0)
                 GL_RGB,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE, // Image data type
                 this->videoFrameCollection[idx].Frame.ptr()
                 );                // The actual image data itself



    return (ImTextureID)(intptr_t)this->videoFrameTexture;


    return 0;

}


void Video::updateFrameCollection(int startFrame, int endFrame) {

    if(startFrame < 0) {
        startFrame = 0;
    }

    if(endFrame >= this->frameCount) {
        endFrame = this->frameCount - 1;
    }

    this->startFrame = startFrame;
    this->endFrame   = endFrame;

    videoReaderTask.SetReadRange(startFrame, endFrame);
    videoReaderTask.Start();

    std::shared_ptr<ReadVideoResult> result = std::static_pointer_cast<ReadVideoResult>(videoReaderTask.GetResult());

    int frmCollSize = result->VideoFrameCollection.size();
    assert(frmCollSize - 1  == endFrame - startFrame);


    int idx = 0;
    for(int i = startFrame; i <= endFrame; i++) {

        VideoFrame frame;
        frame.IsCached       = true;
        frame.Frame          = result->VideoFrameCollection.at(idx).clone();
        //frame.FrameAsTexture = VideoUtils::MatToGLuint(frame.Frame);

        this->videoFrameCollection[i].Clear();
        this->videoFrameCollection[i] = std::move(frame);

        idx++;
    }

    this->emptyCache();
}

bool Video::HasFrames() {
    return this->frameCount > 0;

}

int Video::GetFrameCount() {
    return this->frameCount;
}

void Video::emptyCache() {

    for(int i = 0; i < this->startFrame; i++) {
        if(this->videoFrameCollection[i].IsCached) {
            this->videoFrameCollection[i].Clear();
        }
    }

    for(int i = endFrame + 1; i < this->frameCount; i++) {
        if(this->videoFrameCollection[i].IsCached) {
            this->videoFrameCollection[i].Clear();
        }
    }

}

void Video::initVideoFrameTexture() {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(1, &this->videoFrameTexture);
}
