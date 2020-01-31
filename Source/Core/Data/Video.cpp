#include "Video.h"


#include "Utils/VideoUtils.h"

#include <cassert>
#include <boost/log/trivial.hpp>

Video::Video() {


}

Video::Video(std::string filePath) {
    this->initVideoFrameTexture();

    this->ImportVideoFromPath(filePath);
}

Video::~Video() {
    glDeleteTextures(1, &this->videoFrameTexture);

	
}

std::string Video::GetVideoPath() {
    return this->videoPath;
}

void Video::ImportVideoFromPath(std::string filePath) {
    this->videoPath = filePath;

	this->videoCapture = cv::VideoCapture(filePath);
	if (!this->videoCapture.isOpened()) {
		BOOST_LOG_TRIVIAL(error) << "Failed to open video: " << filePath;
	}

    //videoReaderTask.SetVideoFilePath(this->videoPath);

    this->frameCount = this->videoCapture.get(cv::CAP_PROP_FRAME_COUNT);
	if (this->HasFrames()) {
		videoDim.width = this->videoCapture.get(cv::CAP_PROP_FRAME_WIDTH);
		videoDim.height = this->videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT);
	}

}

cv::Mat Video::GetFrameByIdx(int idx) {

	this->videoCapture.set(cv::CAP_PROP_POS_FRAMES, idx);
	
	cv::Mat frame;
	this->videoCapture.read(frame);

    return frame;
}

ImTextureID Video::GetTextureFrameByIdx(int idx) {

    // Update/Convert texture

	cv::Mat frame = this->GetFrameByIdx(idx);


    glBindTexture(GL_TEXTURE_2D, this->videoFrameTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D,    // Type of texture
		0,                // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,           // Internal colour format to convert to
		frame.cols,       // Image width  i.e. 640 for Kinect in standard mode
		frame.rows,       // Image height i.e. 480 for Kinect in standard mode
		0,                // Border width in pixels (can either be 1 or 0)
		GL_RGB,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE, // Image data type
		frame.ptr()
	);              // The actual image data itself



    return (ImTextureID)(intptr_t)this->videoFrameTexture;


    return 0;

}

bool Video::HasFrames() {
    return this->frameCount > 0;

}

int Video::GetFrameCount() {
    return this->frameCount;
}

void Video::initVideoFrameTexture() {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(1, &this->videoFrameTexture);
}

cv::Size Video::GetVideoDim() {
	
	return this->videoDim;
}