#pragma once

#include <string>

#include "Video.h"
#include "Task/Video/ReadVideo.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include <libmv/autotrack/region.h>
#include <libmv/autotrack/frame_accessor.h>

#include <boost/log/trivial.hpp>


class VideoFrameAccessor : public mv::FrameAccessor {
private:
	Video video;

	

public:
	VideoFrameAccessor();
	VideoFrameAccessor(Video& video);

	void SetVideoSource(Video& video);

	Key GetImage(int clip,
		int frame, // Frame idx
		InputMode input_mode,
		int downscale,               // Downscale by 2^downscale.
		const mv::Region* region,    // Get full image if NULL.
		const Transform* transform,  // May be NULL.
		mv::FloatImage* destination);

	void ReleaseImage(Key);

	Key GetMaskForTrack(int clip,
		int frame,
		int track,
		const mv::Region* region,
		mv::FloatImage* destination);

	void ReleaseMask(Key key);

	bool GetClipDimensions(int clip, int* width, int* height);

	int NumClips();

	int NumFrames(int clip);
};