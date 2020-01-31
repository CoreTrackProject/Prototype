#pragma once

#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <imgui.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}

#include "Task/Video/ReadVideo.h"
#include <GL/glew.h>

#include <libmv/autotrack/region.h>
#include <libmv/autotrack/frame_accessor.h>


#include <boost/log/trivial.hpp>

#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

class Video {

private:
    std::string videoPath;

	cv::Size videoDim;

    // Cursor from where to cache
    int cursorPosition = 0;

    int frameCount = 0;

    int startFrame, endFrame;

    GLuint videoFrameTexture;

	cv::VideoCapture videoCapture;

private:

    void initVideoFrameTexture();

public:
    Video();

    Video(std::string filePath);

    ~Video();
	
public:
    std::string GetVideoPath();

    void ImportVideoFromPath(std::string filePath);

    cv::Mat GetFrameByIdx(int idx);

    ImTextureID GetTextureFrameByIdx(int idx);

    bool HasFrames();	

    int GetFrameCount();

	cv::Size GetVideoDim();
};

class VideoFrameAccessor : public mv::FrameAccessor {
private:
	Video video;


public:
	VideoFrameAccessor() {}
	VideoFrameAccessor(Video& video) : video(video) {}

	void SetVideoSource(Video& video) {
		this->video = video;
	}

	Key GetImage(int clip,
		int frame, // Frame idx
		InputMode input_mode,
		int downscale,               // Downscale by 2^downscale.
		const mv::Region* region,    // Get full image if NULL.
		const Transform* transform,  // May be NULL.
		mv::FloatImage* destination) override {


		// According to Natron LibMv only uses image mono input
		if (input_mode != InputMode::MONO) {
			BOOST_LOG_TRIVIAL(warning) << "input mode is not mono";
		}

		// When region is nullptr then use the whole image else crop image


		
		
		cv::Mat cropImg;
		if (region) {
			cv::Rect2d roi;
			roi.x = region->min(0);
			roi.y = region->min(1);
			roi.width = region->max(0) - region->min(0);
			roi.height = region->max(1) - region->min(1);

			cv::Mat frameMat = this->video.GetFrameByIdx(frame);
			
			cropImg = frameMat(roi); // Crop image by given region
		}
		else {
			cropImg = this->video.GetFrameByIdx(frame);
		}

		

		// Convert every pixel to grayscale
		cv::Mat grayCroppedImg;
		cv::cvtColor(cropImg, grayCroppedImg, cv::COLOR_RGB2GRAY);


		grayCroppedImg.convertTo(grayCroppedImg, CV_32FC1);


		mv::FloatImage mvImage(grayCroppedImg.size().width, grayCroppedImg.size().height);
		float* mvImageDataPtr = mvImage.Data();

		int imgIdx = 0;
		for (int y = 0; y < grayCroppedImg.cols; y++) {
			for (int x = 0; x < grayCroppedImg.rows; x++) {
				
				float pixel = grayCroppedImg.at<float>(x, y);
				mvImageDataPtr[imgIdx] = pixel;
				imgIdx++;
			}
		}
		*destination = mvImage;



		return (mv::FrameAccessor::Key)&mvImage;
	};

	void ReleaseImage(Key) override {

	};

	Key GetMaskForTrack(int clip,
		int frame,
		int track,
		const mv::Region* region,
		mv::FloatImage* destination) override {

		return nullptr;
	};

	void ReleaseMask(Key key) override {

	};

	bool GetClipDimensions(int clip, int* width, int* height) override {

		*width  = this->video.GetVideoDim().width;
		*height = this->video.GetVideoDim().height;

		return this->video.HasFrames();
	};

	int NumClips() override {
		return 1;
	};

	int NumFrames(int clip) override {
		return this->video.GetFrameCount();
	};
};