#pragma once

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>

#include <libmv/autotrack/autotrack.h>

class TrackMarker {

private:

public:

	mv::Marker mvMarker;
    // Describing the lifetime of the tracker
    int BeginFrame = 0;
    int EndFrame   = 0;
	std::vector<mv::Marker> TrackedMarker;
	

public:
    TrackMarker();

	TrackMarker(cv::Point2d center);


public:
    cv::Rect2f GetSearchArea();

	mv::Quad2Df GetMvTrackArea();

    cv::Point2f GetCenterTrackArea();

    TrackMarker Copy();

	mv::Marker GetLibmvMarker();

};

