#pragma once

#include "Data/Video.h"
#include "Data/TrackMarker.h"




struct VideoFrameOCV {
    ImTextureID FrameAsTexture;

    cv::Mat Frame;
    ImTextureID FeatureDrawOverlayTexture;

    std::vector<cv::KeyPoint> KeyPointCollection;
    cv::Mat Descriptor;

    bool HasORB = false;

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
