#pragma once

#include "BaseEditor/IEditorController.hpp"

#include "TrackerEdView.h"
#include "TrackerEdModel.h"

class TrackerEdController : public IEditorController {

private:
    TrackerEdView  view;
    TrackerEdModel model;

public:
    TrackerEdController();
    ~TrackerEdController();


    void Init();
    void DeInit();
    void Render();


};
