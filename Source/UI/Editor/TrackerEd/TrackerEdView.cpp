#include "TrackerEdView.h"
#include "Project/Project.h"
#include "Utils/FileUtils.h"

#include <stb_image.h>

#include <GL/glew.h>

TrackerEdView::TrackerEdView(TrackerEdModel &model) : model(model) {}

TrackerEdView::~TrackerEdView() {}

void TrackerEdView::Init() {


}

void TrackerEdView::Render(){
    ImGui::Begin("Tracker Editor");

    this->drawToolbar();

    ImGui::End();
}

void TrackerEdView::DeInit(){

}

void TrackerEdView::SetAddClipCallback(std::function<void(std::string)> addClipCallback) {
    this->addClipCallback = addClipCallback;
}


void TrackerEdView::SetDetectOrbCallback(std::function<void()> detectOrbCallback) {
    this->detectOrbCallback = detectOrbCallback;
}


void TrackerEdView::drawToolbar() {

    if(ImGui::Button("+", ImVec2(32, 32))) {
        std::string filePath = "";
        if(FileUtils::PickSingleFile(filePath)) {

            this->addClipCallback(filePath);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("ORB", ImVec2(32, 32))) {
        this->detectOrbCallback();
    }


    //ImGui::NewLine();

    ImGui::Separator();

    if (this->model.CurrVideo.HasFrames()) {

        ImGui::Image(
            this->model.CurrVideo.GetTextureFrameByIdx(this->model.VideoSliderValue - 1),
            ImVec2(1280, 720)
            );

        //if (this->model.frameCollection[this->model.VideoSliderValue - 1].HasORB) {
        //
        //
        //	ImGui::Image(
        //		this->model.frameCollection[this->model.VideoSliderValue - 1].FeatureDrawOverlayTexture,
        //		ImVec2(1280, 720)
        //	);
        //} else {
        //	ImGui::Image(
        //		this->model.frameCollection[this->model.VideoSliderValue - 1].FrameAsTexture,
        //		ImVec2(1280, 720)
        //	);
        //}

        ImGui::Separator();

        ImGui::SliderInt(
            "Video",
			&this->model.VideoSliderValue, 
            1,
            this->model.CurrVideo.GetFrameCount()
            );

    }




    // ToDo: Draw line to separate the toolbar



}
