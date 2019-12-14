#include "TrackMarker.h"


TrackMarker::TrackMarker() {
    this->SearchArea = cv::Rect2f(0, 0, 100, 100);
}

TrackMarker::TrackMarker(cv::Rect2f searchArea) {

    this->SearchArea = searchArea;

    cv::Point2f centerSA = this->GetCenterSearchAreaRelative();
    this->TrackArea  = cv::Rect2f(centerSA.x - 10, centerSA.y - 10, 20, 20);

}


cv::Rect2f TrackMarker::GetSearchArea() {
    return this->SearchArea;
}

cv::Rect2f TrackMarker::GetTrackArea() {
    return this->TrackArea;
}

cv::Point2f TrackMarker::GetCenterSearchAreaRelative() {
    return cv::Point2f(
        this->SearchArea.width / 2,
        this->SearchArea.height / 2
        );
}

cv::Point2f TrackMarker::GetCenterTrackArea() {
    return cv::Point2f(
        (this->TrackArea.tl().x + this->TrackArea.br().x) / 2,
        (this->TrackArea.tl().y + this->TrackArea.br().y) / 2
        );
}

void TrackMarker::AdjustSearchAreaToTrackArea(cv::Rect2f trackArea) {

    this->TrackArea = trackArea;

    cv::Point2f posCenterNewTrackAreaRel = cv::Point2f(
        trackArea.br().x - (trackArea.width / 2),
        trackArea.br().y - (trackArea.height / 2));

    cv::Point2f posCenterSearchAreaRel = cv::Point2f(
        this->SearchArea.width / 2,
        this->SearchArea.height / 2);

    cv::Point2f delta = cv::Point2f(posCenterNewTrackAreaRel.x - posCenterSearchAreaRel.x, posCenterNewTrackAreaRel.y - posCenterSearchAreaRel.y);

    this->SearchArea  = cv::Rect2f(this->SearchArea.x + delta.x, this->SearchArea.y + delta.y, this->SearchArea.width, this->SearchArea.height);
}

TrackMarker TrackMarker::Copy() {
    TrackMarker newMarker;

    newMarker.BeginFrame = this->BeginFrame;
    newMarker.EndFrame   = this->EndFrame;

    newMarker.SearchArea = this->SearchArea;
    newMarker.TrackArea  = this->TrackArea;

    return newMarker;
}
