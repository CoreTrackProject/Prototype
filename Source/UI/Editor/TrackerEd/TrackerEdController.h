#pragma once

#include "BaseEditor/IEditorController.hpp"

#include "TrackerEdView.h"
#include "TrackerEdModel.h"

#include <libmv/image/image.h>
#include <libmv/autotrack/region.h>
#include <libmv/autotrack/frame_accessor.h>
#include <libmv/autotrack/autotrack.h>


struct TransformEd : public mv::FrameAccessor::Transform {
	
	virtual ~TransformEd() override {  
	
	}
	
	virtual int64_t key() const override { 
		
		return 0; 
	
	};

	virtual void run(const mv::FloatImage& input, mv::FloatImage* output) const override {

	};
};

struct FrameAccessorEd : public mv::FrameAccessor {



	Key GetImage(int clip,
		int frame,
		InputMode input_mode,
		int downscale,               // Downscale by 2^downscale.
		const mv::Region* region,        // Get full image if NULL.
		const Transform* transform,  // May be NULL.
		mv::FloatImage* destination) override {


		return nullptr;
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
		
		return false; 
	
	};

	int NumClips() override { 
		
		return 1; 
	
	};

	int NumFrames(int clip) override { 
		return 0; 
	};
};










class TrackerEdController : public IEditorController {

private:
	FrameAccessorEd ed;
	

private:
    TrackerEdView  view;
    TrackerEdModel model;

    void addClipCallback(std::string filePath);
    void cacheVideo(std::string filePath);

    void detectORBCallback();
    void trackMarkersCallback();

public:
    TrackerEdController();
    ~TrackerEdController();

    void Init();
    void DeInit();
    void Render();

};
