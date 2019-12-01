#pragma once

#include "Task/ITask.hpp"

#include <string>
#include <boost/filesystem.hpp>

#include <opencv2/core.hpp>


extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}


struct ReadVideoResult {
    bool Successful;
    std::vector<cv::Mat> VideoFrameCollection;
};

// Reading a video and convert it to a opencv mat:
// https://gist.github.com/yohhoy/f0444d3fc47f2bb2d0e2
// https://ffmpeg.org/pipermail/libav-user/2016-October/009801.html
// https://ffmpeg.org/doxygen/trunk/group__lavc__encdec.html


class ReadVideo : public ITask {

private:
    std::string videoFilePath;
	TaskStatus status;

    std::shared_ptr<ReadVideoResult> result;

    int startFrame    = 0;
    int endFrame      = 0;


private:
    bool readVideo();

public:
	ReadVideo();
	~ReadVideo();

	void Start();
	void Stop();
	void Pause();

	TaskStatus GetStatus();
	std::shared_ptr<void> GetResult();

    void SetVideoFilePath(std::string projectPath);

    void SetReadRange(int startFrame, int endFrame);

};
