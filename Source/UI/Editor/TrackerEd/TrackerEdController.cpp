#include "TrackerEdController.h"
#include "Utils/VideoUtils.h"
#include "Task/Video/ReadVideo.h"

#include <imgui.h>
#include <GL/glew.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/tracking.hpp>

#include <libmv/simple_pipeline/pipeline.h>
#include <libmv/tracking/brute_region_tracker.h>




TrackerEdController::TrackerEdController() : view(model) {

    this->view.SetAddClipCallback(std::bind(&TrackerEdController::addClipCallback, this, std::placeholders::_1));
    this->view.SetDetectOrbCallback(std::bind(&TrackerEdController::detectORBCallback, this));
    this->view.SetTrackerMarkersCallback(std::bind(&TrackerEdController::trackMarkersCallback, this));
	this->view.SetAddTrackMarkerCallback(std::bind(&TrackerEdController::addTrackMarkerCallback, this, std::placeholders::_1));
	
	
	
	
}

TrackerEdController::~TrackerEdController() {

}


void TrackerEdController::Init() {
    this->view.Init();
}

void TrackerEdController::DeInit() {
    this->view.DeInit();
}

void TrackerEdController::Render() {
    this->view.Render();
}


void TrackerEdController::addClipCallback(std::string filePath) {
	this->model.CurrVideo.ImportVideoFromPath(filePath);

	
	this->model.sequence.mFrameMax = this->model.CurrVideo.GetFrameCount();


	this->model.sequence.myItems.push_back(
		TrackSequence::TrackSequenceItem{
			TrackSequenceItemType::Clip,
			0,
			this->model.sequence.mFrameMax,
			false,
			"Clip",
			TrackMarker()
		}
	);


}

void TrackerEdController::detectORBCallback() {

}


template<typename QuadT, typename ArrayT>
void QuadToArrays(const QuadT& quad, ArrayT* x, ArrayT* y) {
	for (int i = 0; i < 4; ++i) {
		x[i] = quad.coordinates(i, 0);
		y[i] = quad.coordinates(i, 1);
	}
}


void MarkerToArrays(const mv::Marker& marker, double* x, double* y) {
	mv::Quad2Df offset_quad = marker.patch;
	mv::Vec2f origin = marker.search_region.Rounded().min;
	offset_quad.coordinates.rowwise() -= origin.transpose();
	QuadToArrays(offset_quad, x, y);
	x[4] = marker.center.x() - origin(0);
	y[4] = marker.center.y() - origin(1);
}


void TrackerEdController::trackMarkersCallback() {

	// libmv uses kalman filter
	// https://www.myzhar.com/blog/tutorials/tutorial-opencv-ball-tracker-using-kalman-filter/
    // https://www.youtube.com/watch?v=5SgWfSP6jaM
	// https://github.com/son-oh-yeah/Moving-Target-Tracking-with-OpenCV
	
	VideoFrameAccessor accessor;
	accessor.SetVideoSource(this->model.CurrVideo);

	mv::AutoTrack* autotrack = new mv::AutoTrack(&accessor);
	
	


	
		//autotrack.options.
	
	libmv::vector<mv::Marker> markerCollection;
	for (TrackSequence::TrackSequenceItem sequenceItem : this->model.sequence.myItems) {
		if (sequenceItem.SequenceItemType == TrackSequenceItemType::Track) {
			markerCollection.push_back(sequenceItem.marker.GetLibmvMarker());
		}
	}
	int markerCollectionSize = markerCollection.size();
	autotrack->SetMarkers(&markerCollection);

	


	mv::TrackRegionOptions trackRegionOptions;
	trackRegionOptions.mode = mv::TrackRegionOptions::TRANSLATION;
	trackRegionOptions.minimum_correlation = 0.75;
	trackRegionOptions.sigma = 0.9;
	trackRegionOptions.max_iterations = 50;
	trackRegionOptions.use_brute_initialization = true;
	trackRegionOptions.use_normalized_intensities = false;
	trackRegionOptions.use_esm = true;
	trackRegionOptions.attempt_refine_before_brute = false;
	trackRegionOptions.num_extra_points = 1;
	trackRegionOptions.regularization_coefficient = 0.0;
	trackRegionOptions.minimum_corner_shift_tolerance_pixels = 0.005;



	// Test
	{
		TrackMarker ctMarker = this->model.sequence.myItems[1].marker;

		mv::Marker refMarker     = ctMarker.mvMarker;
		mv::Marker markerToTrack = ctMarker.mvMarker;

		for (int frameIdx = 0; frameIdx < this->model.CurrVideo.GetFrameCount(); frameIdx++) {
			
			mv::FloatImage image1(100, 100, 1);
			accessor.GetImage(
				0,
				0, // Frame 0
				mv::FrameAccessor::MONO,
				0,
				&refMarker.search_region.Rounded(),
				nullptr,
				&image1);


			mv::FloatImage image2(100, 100, 1);
			accessor.GetImage(
				0,
				frameIdx + 1, // Frame 1
				mv::FrameAccessor::MONO,
				0,
				&markerToTrack.search_region.Rounded(),
				nullptr,
				&image2);


			double x1[5];
			double y1[5];
			MarkerToArrays(refMarker, x1, y1);

			double x2[5]; // New pos of the patch
			double y2[5];
			MarkerToArrays(markerToTrack, x2, y2);




			mv::Vec2f original_center = markerToTrack.center;

			mv::TrackRegionResult res;
			//libmv::TrackRegion(image1, image2, x1, y1, trackRegionOptions, x2, y2, &res);

			libmv::BruteRegionTracker regionTracker;
			regionTracker.minimum_correlation = 0.75;
			regionTracker.half_window_size = 20;
			bool result = regionTracker.Track(image1, image2, x1[4], y1[4], x2, y2);

			if (!result) {
				this->model.sequence.myItems[1].marker.EndFrame = frameIdx;
				this->model.sequence.myItems[1].mFrameEnd = frameIdx;
				break;
			}



			// Update and add markerToTrack to the list
			// Copy results over the tracked marker.
			//mv::Vec2f tracked_origin = markerToTrack.search_region.Rounded().min;
			//for (int i = 0; i < 4; ++i) {
			//	markerToTrack.patch.coordinates(i, 0) = x2[i] + tracked_origin[0];
			//	markerToTrack.patch.coordinates(i, 1) = y2[i] + tracked_origin[1];
			//}


			//markerToTrack.center(0) = x2[4] + tracked_origin[0];
			//markerToTrack.center(1) = y2[4] + tracked_origin[1];

			//mv::Vec2f delta = markerToTrack.center - original_center;


			//markerToTrack.search_region.Offset(delta);


			markerToTrack.center(0) = markerToTrack.center(0) + x2[0];
			markerToTrack.center(1) = markerToTrack.center(1) + y2[0];

			double deltaX = x2[0] - x1[4];
			double deltaY = y2[0] - y1[4];


			

			markerToTrack.source = mv::Marker::TRACKED;
			markerToTrack.status = mv::Marker::UNKNOWN;
			markerToTrack.reference_clip = refMarker.clip;
			markerToTrack.reference_frame = refMarker.frame;



			this->model.sequence.myItems[1].marker.TrackedMarker.push_back(markerToTrack);

			//refMarker = markerToTrack;


			if (!res.is_usable()) {
				//this->model.sequence.myItems[1].marker.EndFrame = frameIdx;
				//this->model.sequence.myItems[1].mFrameEnd = frameIdx;
				//break;
			}


		}

	}
	
	return;



	// For each marker
	
	for (TrackSequence::TrackSequenceItem &sequenceItem : this->model.sequence.myItems) {

		if (sequenceItem.SequenceItemType != TrackSequenceItemType::Track) {
			continue;
		}
		int markerIdx = 0;
		for (int frameIdx = 0; frameIdx < this->model.CurrVideo.GetFrameCount(); frameIdx++) {

			// Check if frame has marker by index
			mv::Marker currentMarker;
			bool hasMarker = autotrack->GetMarker(
				0,
				frameIdx, 
				markerIdx,
				&currentMarker);

			// current frame doesnt have markers which are placed by the user so check next frame
			if (!hasMarker) {
				continue;
			}

			// -----
			// TODO: Check if marker is in bounds
			// -----

			mv::Marker trackedMarker      = currentMarker; // trackedMarker a new marker which is created on the next frame
			trackedMarker.frame           = frameIdx + 1; // Next frame to track 
			trackedMarker.reference_frame = currentMarker.reference_frame;

			mv::TrackRegionResult result;

			bool status = autotrack->TrackMarker(&trackedMarker, &result, &trackRegionOptions);

			if (status) {
				autotrack->AddMarker(trackedMarker);
				sequenceItem.marker.TrackedMarker.push_back(trackedMarker);
				sequenceItem.mFrameEnd = frameIdx;
				sequenceItem.marker.EndFrame = frameIdx;
			}
			else {
				sequenceItem.mFrameEnd = frameIdx;
				sequenceItem.marker.EndFrame = frameIdx;

				break;
			}


			// According to the implementation of AddMarker it add a new marker instance to the vector in autotrack
			// it assigns it the vector  -> markers_[i] = marker; -> it updates the existing marker 
			
			

		}

		markerIdx++;
	}



	delete autotrack;




}

void TrackerEdController::addTrackMarkerCallback(TrackMarker marker) {
	this->model.sequence.myItems.push_back(
		TrackSequence::TrackSequenceItem { 
			TrackSequenceItemType::Track,
			marker.BeginFrame,
			marker.EndFrame,
			false, 
			"Track " + std::to_string(this->model.sequence.myItems.size()), 
			std::move(marker) 
		}
	);

}