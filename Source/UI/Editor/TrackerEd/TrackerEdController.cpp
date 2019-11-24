#include "TrackerEdController.h"
#include "Project/Project.h"

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
	

    //this->view.SetAddClipCallback(this->addClipCallback)
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
    cv::VideoCapture video(filePath);

    cv::Mat videoFrame;
    ImTextureID videoFrameGlTexture = 0;
	int frameNr = 1;

	if (this->model.frameCollection.size() > 0) {
		this->model.frameCollection.clear();
		
	}

	this->model.frameCollection.reserve(video.get(cv::CAP_PROP_FRAME_COUNT));

    while (video.read(videoFrame)) {
		
		videoFrameGlTexture = this->matToTexture(videoFrame);

		BOOST_LOG_TRIVIAL(debug) << "Frame cached " << frameNr++ << "/" << video.get(cv::CAP_PROP_FRAME_COUNT);

		VideoFrameOCV frameOCV;
		frameOCV.Frame = std::move(videoFrame);
		frameOCV.FrameAsTexture = std::move(videoFrameGlTexture);
		cv::cvtColor(frameOCV.Frame, frameOCV.FrameGray, cv::COLOR_BGR2GRAY);

		this->model.frameCollection.push_back(std::move(frameOCV));

    }

}

ImTextureID TrackerEdController::matToTexture(cv::Mat frame) {
	GLuint videoFrameGlTexture;
	cv::Mat frameGray;

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(1, &videoFrameGlTexture);
	glBindTexture(GL_TEXTURE_2D, videoFrameGlTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	cv::cvtColor(frame, frameGray, cv::COLOR_RGB2BGR);

	glTexImage2D(GL_TEXTURE_2D,         // Type of texture
		0,                   // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,              // Internal colour format to convert to
		frameGray.cols,          // Image width  i.e. 640 for Kinect in standard mode
		frameGray.rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                   // Border width in pixels (can either be 1 or 0)
		GL_RGB,              // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,    // Image data type
		frameGray.ptr()
	);        // The actual image data itself

	return (ImTextureID)(intptr_t)videoFrameGlTexture;

}

void TrackerEdController::generateOverlay(int frameIdx) {
	
	cv::Mat frame = this->model.frameCollection[frameIdx].Frame;
	cv::Mat overlay = this->model.frameCollection[frameIdx].FeatureDrawOverlay;
	cv::Mat result;

	cv::addWeighted(frame, 1, overlay, 1, 1, result);

	this->model.frameCollection[frameIdx].FeatureDrawOverlayTexture =
		this->matToTexture(result);

}