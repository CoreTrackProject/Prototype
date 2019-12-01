#include "Video.h"


#include "Utils/VideoUtils.h"

#include <cassert>


Video::Video() {


}


Video::Video(std::string filePath) {
    this->ImportVideoFromPath(filePath);
}

Video::~Video() {}


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

ImTextureID Video::GetTextureFrameByIdx(int idx, bool cacheForward){
    if(!this->videoFrameCollection[idx].IsCached) {
        if(cacheForward) {
            this->updateFrameCollection(idx, idx + 100);
        } else {
            this->updateFrameCollection(idx - 50, idx + 50);
        }
    }
    return (ImTextureID)(intptr_t)this->videoFrameCollection[idx].FrameAsTexture;
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
        frame.FrameAsTexture = VideoUtils::MatToGLuint(frame.Frame);

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
