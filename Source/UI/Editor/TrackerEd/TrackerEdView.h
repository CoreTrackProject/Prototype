#pragma once

#include <BaseEditor/IEditorView.hpp>

#include "Project/ProjectHeader.h"
#include "TrackerEdModel.h"

#include <imgui.h>


// https://www.learnopencv.com/image-alignment-feature-based-using-opencv-c-python/
// https://www.youtube.com/watch?v=sHHvORwgFQ4
// https://github.com/opencv/opencv/blob/master/samples/python/plane_tracker.py#L74
// https://www.youtube.com/watch?v=pzVbhxx6aog

class TrackerEdView : IEditorView {

private:
    //ImTextureID importFileTex;

	// this shall be exported to a cache engine (TODO)
	// invocation for caching also in TrackerEdController
	

    Content currVideoFile;
	TrackerEdModel &model;

public:
	std::function<void(std::string)> addClipCallback;
	std::function<void()> detectOrbCallback;

public:
	TrackerEdView(TrackerEdModel &model);
    ~TrackerEdView();

    void Init();
    void Render();
    void DeInit();

	void SetAddClipCallback(std::function<void(std::string)> addClipCallback);
	void SetDetectOrbCallback(std::function<void()> detectOrbCallback);

private:
    void drawToolbar();

};
