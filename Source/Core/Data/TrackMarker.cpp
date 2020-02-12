#include "TrackMarker.h"


TrackMarker::TrackMarker() {

}

TrackMarker::TrackMarker(cv::Point2d center) {
	

	this->mvMarker.clip = 0;
	this->mvMarker.reference_clip = 0;
	this->mvMarker.model_type = mv::Marker::POINT;
	this->mvMarker.model_id = 0;
	this->mvMarker.track = 0;
	this->mvMarker.weight = 1.0;
	this->mvMarker.frame = 0;
	this->mvMarker.reference_frame = 0;
	this->mvMarker.disabled_channels = 0;
	this->mvMarker.status = mv::Marker::Status::UNKNOWN;
	this->mvMarker.source = mv::Marker::Source::MANUAL;




	this->mvMarker.center(0) = center.x;
	this->mvMarker.center(1) = center.y;

	// Search area
	this->mvMarker.search_region.min(0) = center.x - 50;
	this->mvMarker.search_region.min(1) = center.y - 50;

	this->mvMarker.search_region.max(0) = center.x + 50;
	this->mvMarker.search_region.max(1) = center.y + 50;

	// Track area
	this->mvMarker.patch.coordinates(0, 0) = center.x - 25;
	this->mvMarker.patch.coordinates(0, 1) = center.y - 25;

	this->mvMarker.patch.coordinates(1, 0) = center.x + 25;
	this->mvMarker.patch.coordinates(1, 1) = center.y - 25;

	this->mvMarker.patch.coordinates(2, 0) = center.x + 25;
	this->mvMarker.patch.coordinates(2, 1) = center.y + 25;

	this->mvMarker.patch.coordinates(3, 0) = center.x - 25;
	this->mvMarker.patch.coordinates(3, 1) = center.y + 25;


}

cv::Rect2f TrackMarker::GetSearchArea() {

	cv::Rect2d rect;
	rect.x = this->mvMarker.search_region.min(0);
	rect.y = this->mvMarker.search_region.min(1);

	rect.width  = this->mvMarker.search_region.max(0) -
		this->mvMarker.search_region.min(0);

	rect.height = this->mvMarker.search_region.max(1) -
		this->mvMarker.search_region.min(1);

    return rect;
}

mv::Quad2Df TrackMarker::GetMvTrackArea() {
	return this->mvMarker.patch;
}

cv::Point2f TrackMarker::GetCenterTrackArea() {

	cv::Point2f point = cv::Point2f(
		this->mvMarker.center(0),
		this->mvMarker.center(1)
	);

	return point;
}

TrackMarker TrackMarker::Copy() {
    TrackMarker newMarker;

    newMarker.BeginFrame = this->BeginFrame;
    newMarker.EndFrame   = this->EndFrame;



    return newMarker;
}

mv::Marker TrackMarker::GetLibmvMarker() {
	return this->mvMarker;
}