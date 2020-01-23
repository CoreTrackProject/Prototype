#include "TrackerEdController.h"
#include "Project/Project.h"
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




TrackerEdController::TrackerEdController() : view(model) {

    this->view.SetAddClipCallback(std::bind(&TrackerEdController::addClipCallback, this, std::placeholders::_1));
    this->view.SetDetectOrbCallback(std::bind(&TrackerEdController::detectORBCallback, this));
    this->view.SetTrackerMarkersCallback(std::bind(&TrackerEdController::trackMarkersCallback, this));


	mv::AutoTrack track(&ed);



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
    Project::GetInstance().ImportFile(filePath);

    this->cacheVideo(filePath);
}

void TrackerEdController::detectORBCallback() {

    for(TrackMarker &marker : this->model.TrackMarkerCollection) {

        cv::Ptr<cv::Feature2D> feature = cv::ORB::create(10);

        for (int idx = 0; idx < this->model.FrameCollection.size(); idx++) {

            this->model.FrameCollection[idx].HasORB = true;

            BOOST_LOG_TRIVIAL(debug) << "Frame processed " << idx << "/" << this->model.FrameCollection.size();


        }
    }



}

void TrackerEdController::trackMarkersCallback() {

    for(TrackMarker &marker : this->model.TrackMarkerCollection) {

        cv::Ptr<cv::Tracker> ocvTracker = cv::TrackerCSRT::create();
        cv::Mat currFrame;
        cv::Rect2d trackRect = marker.GetTrackArea();

        for(int frameIdx = marker.BeginFrame; frameIdx <  this->model.CurrVideo.GetFrameCount(); frameIdx++) {

            currFrame = this->model.CurrVideo.GetFrameByIdx(frameIdx, true).clone();

            cv::Rect2d rect = marker.GetSearchArea();
            // Create search area
            currFrame = currFrame(rect);


            if (frameIdx == 0) {
                bool initSucess = ocvTracker->init(currFrame, trackRect);
                if(!initSucess) {
                    BOOST_LOG_TRIVIAL(error) << "Tracker init failed";
                    break;

                } else {
                    marker.Anim.push_back(marker.Copy());
                }

            } else {
                bool trackingSuccess = ocvTracker->update(currFrame, trackRect);
                if(!trackingSuccess) {

                    BOOST_LOG_TRIVIAL(error) << "Tracker update failed on frame: " << frameIdx;
                    marker.EndFrame = frameIdx - 1; // -1 because the tracker failed on the current frame
                    break;

                } else {

                    // Update tracker
                    marker.AdjustSearchAreaToTrackArea(trackRect);

                    // Add save transformations
                    marker.Anim.push_back(marker.Copy());

                }
            }

        }
    }

}

void TrackerEdController::cacheVideo(std::string filePath) {
    this->model.CurrVideo.ImportVideoFromPath(filePath);
}
