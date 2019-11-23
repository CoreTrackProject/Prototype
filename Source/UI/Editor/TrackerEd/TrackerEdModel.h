#pragma once


#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <imgui.h>

class TrackerEdModel {
public:
    int VideoSliderValue = 0;
    int CurrFrameArrIdx = 0;

    std::vector<ImTextureID> ClipTextureCollection;
    std::vector<cv::Mat> VideoOCVMatCollection;

public:
    TrackerEdModel();
    ~TrackerEdModel();

};
