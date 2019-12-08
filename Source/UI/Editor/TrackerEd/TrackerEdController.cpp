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


#include <libmv/tracking/klt_region_tracker.h>




TrackerEdController::TrackerEdController() : view(model) {

    this->view.SetAddClipCallback(std::bind(&TrackerEdController::addClipCallback, this, std::placeholders::_1));
    this->view.SetDetectOrbCallback(std::bind(&TrackerEdController::detectORBCallback, this));
    this->view.SetTrackerMarkersCallback(std::bind(&TrackerEdController::trackMarkersCallback, this));



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

    cv::Ptr<cv::Feature2D> feature = cv::ORB::create(50);
    for (int idx = 0; idx < this->model.FrameCollection.size(); idx++) {

        feature->detectAndCompute(
            this->model.FrameCollection[idx].FrameGray,
            cv::Mat(),
            this->model.FrameCollection[idx].KeyPointCollection,
            this->model.FrameCollection[idx].Descriptor
            );

        cv::drawKeypoints(
            this->model.FrameCollection[idx].Frame,
            this->model.FrameCollection[idx].KeyPointCollection,
            this->model.FrameCollection[idx].FeatureDrawOverlay);

        this->model.FrameCollection[idx].HasORB = true;

        this->generateOverlay(idx);

        BOOST_LOG_TRIVIAL(debug) << "Frame processed " << idx << "/" << this->model.FrameCollection.size();
    }



}

void TrackerEdController::trackMarkersCallback() {

    libmv::KltRegionTracker *tracker = new libmv::KltRegionTracker();

    // Convert cv::Mat to float array

    //cv::Mat firstImg = VideoUtils::MatToFloatArray(this->model.CurrVideo.GetFrameByIdx(0));
    //cv::Mat secImg = this->model.CurrVideo.GetFrameByIdx(0);

    //libmv::FloatImage img1 = VideoUtils::MatToFloatImage(this->model.CurrVideo.GetFrameByIdx(0));
    //libmv::FloatImage img2 = VideoUtils::MatToFloatImage(this->model.CurrVideo.GetFrameByIdx(1));

    // FloatImage RGB


    for(TrackMarker marker : this->model.TrackMarkerCollection) {

        libmv::FloatImage currImg;
        libmv::FloatImage lastImg;

        double newPosX = 0;
        double newPosY = 0;


        for(int frameIdx = 0; frameIdx < this->model.CurrVideo.GetFrameCount(); frameIdx++) {
            cv::Mat frameMat = this->model.CurrVideo.GetFrameByIdx(frameIdx, true).clone();


            cv::Mat frameRect = frameMat(marker.GetOCVRect());
            libmv::FloatImage currImage = VideoUtils::MatToFloatImage(frameRect);

            if(frameIdx > 0) {

                if(tracker->Track(currImg, lastImg, marker.CenterX, marker.CenterY, &newPosX, &newPosY)) {
                    BOOST_LOG_TRIVIAL(info) << "Track successful moving to next frame " << frameIdx;


                } else {
                    break;
                }

            }

            lastImg = currImg;
        }
    }




    double x       = 603;
    double y       = 835;
    double newPosX = 0;
    double newPosY = 0;
    cv::Mat currMat;


/*    for(int i = 0; i < this->model.CurrVideo.GetFrameCount(); i++) {

        currMat = this->model.CurrVideo.GetFrameByIdx(i, true).clone();
        currImg = VideoUtils::MatToFloatImage(currMat);

        // Curr and last image var is set
        if(i > 0) {

            // When tracker returns false then abort
            if(tracker.Track(currImg, lastImg, x, y, &newPosX, &newPosY)) {

                // Update new x and y
                x = newPosX;
                y = newPosY;

            } else {
                break;
            }
        }

        lastImg = currImg;
    }*/

}

void TrackerEdController::cacheVideo(std::string filePath) {
    this->model.CurrVideo.ImportVideoFromPath(filePath);
}

void TrackerEdController::generateOverlay(int frameIdx) {

    //cv::Mat frame = this->model.frameCollection[frameIdx].Frame;
    //cv::Mat overlay = this->model.frameCollection[frameIdx].FeatureDrawOverlay;
    //cv::Mat result;
    //
    //cv::addWeighted(frame, 1, overlay, 1, 1, result);
    //
    //this->model.frameCollection[frameIdx].FeatureDrawOverlayTexture =
    //    VideoUtils::MatToImTextureID(result);
    //

}
