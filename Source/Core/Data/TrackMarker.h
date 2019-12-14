#pragma once

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>

class TrackMarker {

private:
    cv::Rect2f SearchArea;

    // TrackArea is relative to SearchArea
    cv::Rect2f TrackArea;

public:

    // Describing the lifetime of the tracker
    int BeginFrame = 0;
    int EndFrame   = 0;

    // Stores the transformation of a tracker during tracking process
    std::vector<TrackMarker> Anim;

public:
    TrackMarker();

    TrackMarker(cv::Rect2f searchArea);

    TrackMarker(cv::Rect2f searchArea, cv::Rect2f trackArea);

    cv::Rect2f GetSearchArea();

    cv::Rect2f GetTrackArea();

    cv::Point2f GetCenterSearchAreaRelative();

    cv::Point2f GetCenterTrackArea();

    void AdjustSearchAreaToTrackArea(cv::Rect2f trackArea);

    TrackMarker Copy();

};

