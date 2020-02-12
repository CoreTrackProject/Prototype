#include "Video.h"


#include "Utils/VideoUtils.h"

#include <cassert>
#include <boost/log/trivial.hpp>
#include "VideoFrameAccessor.h"

VideoFrameAccessor::VideoFrameAccessor() {

}

VideoFrameAccessor::VideoFrameAccessor(Video& video) : video(video) {

}

void VideoFrameAccessor::SetVideoSource(Video& video) {
	this->video = video;
}

mv::FrameAccessor::Key VideoFrameAccessor::GetImage(int clip,
	int frame, // Frame idx
	InputMode input_mode,
	int downscale,               // Downscale by 2^downscale.
	const mv::Region* region,    // Get full image if NULL.
	const Transform* transform,  // May be NULL.
	mv::FloatImage* destination) {

	// TODO: Check the image coordinates of FloatImage
	// opencv mat it is top left corner is 0/0
	// floatimage is probably (assumption) bottom left


	// According to Natron LibMv only uses image mono input
	if (input_mode != InputMode::MONO) {
		BOOST_LOG_TRIVIAL(warning) << "input mode is not mono";
	}

	// When region is nullptr then use the whole image else crop image

	cv::Mat cropImg;
	if (region) {
		cv::Rect2d roi;
		roi.x      = region->min(0);
		roi.y      = region->min(1);
		roi.width  = region->max(0) - region->min(0);
		roi.height = region->max(1) - region->min(1);

		cv::Mat frameMat = this->video.GetFrameByIdx(frame);
		cropImg = frameMat(roi); // Crop image by given region
		
	} else {
		// Get full image
		cropImg = this->video.GetFrameByIdx(frame);
	}


	


	std::vector<float> floatRect;
	floatRect.reserve(cropImg.rows * cropImg.cols);

	for (int y = 0; y < cropImg.rows; y++)
	{
		for (int x = 0; x < cropImg.cols; x++)
		{
			cv::Vec3b pixel = cropImg.at<cv::Vec3b>(x, y);

			// Convert to float value
			float r = static_cast<float>(pixel.val[0])  * (1.0f / 255.0f);
			float g = static_cast<float>(pixel.val[1])  * (1.0f / 255.0f);
			float b = static_cast<float>(pixel.val[2])  * (1.0f / 255.0f);

			// Convert to gray scale and add to float vector
			floatRect.push_back(
				0.2126f * r +
				0.7152f * g +
				0.0722f * b);

		}
	}

	mv::FloatImage mvFinalImage(
		floatRect.data(),
		cropImg.size().width,
		cropImg.size().height);

	destination->CopyFrom(mvFinalImage);





	return (mv::FrameAccessor::Key)&mvFinalImage;

	// ----------------------------------

	// Convert every pixel to grayscale
	cv::Mat grayCroppedImg;
	{
		cv::cvtColor(cropImg, grayCroppedImg, cv::COLOR_RGB2GRAY);
		grayCroppedImg.convertTo(grayCroppedImg, CV_32FC1);

		if (downscale > 0) {
			cv::Size newSize(
				grayCroppedImg.size().width  / (1 << downscale),
				grayCroppedImg.size().height / (1 << downscale)
			);
			cv::resize(grayCroppedImg,
				grayCroppedImg,
				newSize,
				0,
				0,
				cv::INTER_AREA
			);
		}
	}

	mv::FloatImage mvImage(
		grayCroppedImg.size().width, 
		grayCroppedImg.size().height);

	float* mvImageDataPtr = mvImage.Data();

	for (int y = 0; y < grayCroppedImg.cols; y++) {
		for (int x = 0; x < grayCroppedImg.rows; x++) {
			mvImage.Fill(grayCroppedImg.at<float>(x, y));
		}
	}

	//*destination = mvImage;
	destination->CopyFrom(mvImage);


	return (mv::FrameAccessor::Key)&mvImage;
};

void VideoFrameAccessor::ReleaseImage(Key) {

};

mv::FrameAccessor::Key VideoFrameAccessor::GetMaskForTrack(int clip,
	int frame,
	int track,
	const mv::Region* region,
	mv::FloatImage* destination) {

	return nullptr;
};

void VideoFrameAccessor::ReleaseMask(Key key) {

};

bool VideoFrameAccessor::GetClipDimensions(int clip, int* width, int* height) {

	*width = this->video.GetVideoDim().width;
	*height = this->video.GetVideoDim().height;

	return this->video.HasFrames();
};

int VideoFrameAccessor::NumClips() {
	return 1;
};

int VideoFrameAccessor::NumFrames(int clip) {
	return this->video.GetFrameCount();
};