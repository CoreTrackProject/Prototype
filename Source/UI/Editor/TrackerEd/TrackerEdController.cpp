#include "TrackerEdController.h"
#include "Project/Project.h"
#include "Utils/VideoUtils.h"
#include "Task/Video/ReadVideo.h"

#include <imgui.h>
#include <GL/glew.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/imgproc/imgproc.hpp>

#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>





TrackerEdController::TrackerEdController() : view(model) {
    this->view.SetAddClipCallback(std::bind(&TrackerEdController::addClipCallback, this, std::placeholders::_1));
    this->view.SetDetectOrbCallback(std::bind(&TrackerEdController::detectORBCallback, this));

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
    for (int idx = 0; idx < this->model.frameCollection.size(); idx++) {

        feature->detectAndCompute(
            this->model.frameCollection[idx].FrameGray,
            cv::Mat(),
            this->model.frameCollection[idx].KeyPointCollection,
            this->model.frameCollection[idx].Descriptor
            );

        cv::drawKeypoints(
            this->model.frameCollection[idx].Frame,
            this->model.frameCollection[idx].KeyPointCollection,
            this->model.frameCollection[idx].FeatureDrawOverlay);

        this->model.frameCollection[idx].HasORB = true;

        this->generateOverlay(idx);

        BOOST_LOG_TRIVIAL(debug) << "Frame processed " << idx << "/" << this->model.frameCollection.size();
    }



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
