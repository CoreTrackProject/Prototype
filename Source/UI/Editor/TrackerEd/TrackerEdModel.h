#pragma once

#include "Data/Video.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>

#include <imgui.h>

#include <cmath>

struct TrackMarker {

    int CenterX, CenterY;

    int Width, Height;

    bool Selected;

    cv::Rect GetOCVRect() {
        return cv::Rect(
            this->CenterX - round((this->Width / 2)),
            this->CenterY - round((this->Height / 2)),
            this->Width,
            this->Height);
    }

};



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

    std::vector<VideoFrameOCV> FrameCollection;

    std::vector<TrackMarker> TrackMarkerCollection;

    Video CurrVideo;

public:
    TrackerEdModel();
    ~TrackerEdModel();

};
