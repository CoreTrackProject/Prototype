#pragma once

#include "Data/Video.h"
#include "Data/TrackMarker.h"


class TrackerEdModel {

public:
    int VideoSliderValue = 1;
    int CurrFrameArrIdx  = 0;

	Video CurrVideo;

    std::vector<TrackMarker> TrackMarkerCollection;

	
public:
    TrackerEdModel();
    ~TrackerEdModel();

};
