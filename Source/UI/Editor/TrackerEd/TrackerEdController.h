#pragma once

#include "BaseEditor/IEditorController.hpp"

#include "TrackerEdView.h"
#include "TrackerEdModel.h"

#include <libmv/image/image.h>

class TrackerEdController : public IEditorController {

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
