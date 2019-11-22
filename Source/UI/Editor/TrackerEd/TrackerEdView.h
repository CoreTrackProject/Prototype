#pragma once

#include <BaseEditor/IEditorView.hpp>

#include "Project/ProjectHeader.h"

#include <imgui.h>
// https://www.learnopencv.com/image-alignment-feature-based-using-opencv-c-python/
//https://www.youtube.com/watch?v=sHHvORwgFQ4

#include "TrackerEdModel.h"

class TrackerEdView : IEditorView {

private:
    //ImTextureID importFileTex;

	


	// this shall be exported to a cache engine (TODO)
	// invocation for caching also in TrackerEdController
	

    Content currVideoFile;
	TrackerEdModel &model;

public:
	std::function<void(std::string)> addClipCallback;

public:
    TrackerEdView();
	TrackerEdView(TrackerEdModel &model);
    ~TrackerEdView();

    void Init();
    void Render();
    void DeInit();

	void SetAddClipCallback(std::function<void(std::string)> addClipCallback);

private:
    void drawToolbar();

	void cacheVideo(std::string filePath);
	void detectORB(int frameIdx);

};
