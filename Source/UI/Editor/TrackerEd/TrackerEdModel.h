#pragma once

#include "Data/Video.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>

#include <imgui.h>




struct VideoFrameOCV {
	ImTextureID FrameAsTexture;

    cv::Mat Frame;
	cv::Mat FrameGray;
	cv::Mat FeatureDrawOverlay;
	ImTextureID FeatureDrawOverlayTexture;

	std::vector<cv::KeyPoint> KeyPointCollection;
	cv::Mat Descriptor;

	bool HasORB = false;

};

struct FeatureTrack {
	std::vector<cv::Point2d> TrackerPath;
};



class TrackerEdModel {

public:
    int VideoSliderValue = 1;
    int CurrFrameArrIdx = 0;

    std::vector<VideoFrameOCV> frameCollection;

    Video CurrVideo;

public:
    TrackerEdModel();
    ~TrackerEdModel();

};
