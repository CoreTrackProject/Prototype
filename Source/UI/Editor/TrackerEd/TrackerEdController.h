#pragma once

#include "BaseEditor/IEditorController.hpp"

#include "TrackerEdView.h"
#include "TrackerEdModel.h"

class TrackerEdController : public IEditorController {

private:
    TrackerEdView  view;
    TrackerEdModel model;


    void addClipCallback(std::string filePath);
    void cacheVideo(std::string filePath);
    void detectORB(int frameIdx);

public:
    TrackerEdController();
    ~TrackerEdController();


    void Init();
    void DeInit();
    void Render();




};
