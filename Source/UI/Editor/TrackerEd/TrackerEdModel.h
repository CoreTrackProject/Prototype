#pragma once


#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class TrackerEdModel {
private:
	int currFrameArrIdx = 0;
	std::vector<ImTextureID> clipTextureCollection;
	std::vector<cv::Mat> videoOCVMatCollection;

public:
    TrackerEdModel();

    ~TrackerEdModel();


};
