#pragma once

#include "BaseEditor/IEditorController.hpp"

#include "TrackerEdView.h"
#include "TrackerEdModel.h"

#include <libmv/image/image.h>
#include <libmv/autotrack/region.h>
#include <libmv/autotrack/frame_accessor.h>
#include <libmv/autotrack/autotrack.h>


class TrackerEdController : public IEditorController {	

private:
    TrackerEdView  view;
    TrackerEdModel model;

	VideoFrameAccessor accessor;
	std::unique_ptr<mv::AutoTrack> autotrack;

    void addClipCallback(std::string filePath);
    void cacheVideo(std::string filePath);

    void detectORBCallback();
    void trackMarkersCallback();
	void addTrackMarkerCallback(TrackMarker marker);

public:
    TrackerEdController();
    ~TrackerEdController();

    void Init();
    void DeInit();
    void Render();

};
