#include "TrackerEdView.h"
#include "Project/Project.h"
#include "Utils/File.h"

#include <stb_image.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <GL/glew.h>

TrackerEdView::TrackerEdView(TrackerEdModel &model) : model(model) {}

TrackerEdView::TrackerEdView() : model(TrackerEdModel()) {}

TrackerEdView::~TrackerEdView() {}

void TrackerEdView::Init() {


}

void TrackerEdView::Render(){
    ImGui::Begin("Tracker Editor");

    this->drawToolbar();

    ImGui::End();
}

void TrackerEdView::DeInit(){

}

void TrackerEdView::SetAddClipCallback(std::function<void(std::string)> addClipCallback) {
	this->addClipCallback = addClipCallback;
}


void TrackerEdView::drawToolbar() {

    if(ImGui::Button("+", ImVec2(32, 32))) {
        std::string filePath = "";
        if(File::PickSingleFile(filePath)) {
            Project::GetInstance().ImportFile(filePath);

			this->cacheVideo(filePath);
        }
    }
	
	ImGui::SameLine();

	if (ImGui::Button("ORB", ImVec2(32, 32))) {


	}

	ImGui::NewLine();



	//if (this->clipTextureCollection.size() > 2) {
		//ImGui::Image(this->clipTextureCollection[1], ImVec2(1280, 720));
	//}

    ImGui::Separator();
    // ToDo: Draw line to separate the toolbar



}

void TrackerEdView::cacheVideo(std::string filePath) {

	cv::VideoCapture video(filePath);

	cv::Mat videoFrame;
	GLuint videoFrameGlTexture = 0;
	while (video.read(videoFrame)) {
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glGenTextures(1, &videoFrameGlTexture);
		glBindTexture(GL_TEXTURE_2D, videoFrameGlTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		cv::cvtColor(videoFrame, videoFrame, cv::COLOR_RGB2BGR);

		glTexImage2D(GL_TEXTURE_2D,         // Type of texture
			0,                   // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,              // Internal colour format to convert to
			videoFrame.cols,          // Image width  i.e. 640 for Kinect in standard mode
			videoFrame.rows,          // Image height i.e. 480 for Kinect in standard mode
			0,                   // Border width in pixels (can either be 1 or 0)
			GL_RGB,              // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE,    // Image data type
			videoFrame.ptr());        // The actual image data itself

		//this->clipTextureCollection.push_back((ImTextureID)(intptr_t)videoFrameGlTexture);
	}

}

void TrackerEdView::detectORB(int frameIdx) {
	
}