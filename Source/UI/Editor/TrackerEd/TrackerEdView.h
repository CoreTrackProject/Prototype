#pragma once

#include <BaseEditor/IEditorView.hpp>

#include "Project/ProjectHeader.h"

#include <imgui.h>


class TrackerEdView : IEditorView {

private:
    ImTextureID importFileTex;

    Content currVideoFile;

public:
    TrackerEdView();
    ~TrackerEdView();

    void Init();
    void Render();
    void DeInit();

private:
    void drawToolbar();


};
