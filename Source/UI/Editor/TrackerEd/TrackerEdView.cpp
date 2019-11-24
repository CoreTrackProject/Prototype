#include "TrackerEdView.h"
#include "Project/Project.h"
#include "Utils/File.h"

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
        if(File::PickSingleFile(filePath)) {

            this->addClipCallback(filePath);
            //


            //this->cacheVideo(filePath);
        }
    }
	
	ImGui::SameLine();

	if (ImGui::Button("ORB", ImVec2(32, 32))) {
		this->detectOrbCallback();
	}

	ImGui::NewLine();



    if (this->model.frameCollection.size() > 0) {

		if (this->model.frameCollection[this->model.VideoSliderValue - 1].HasORB) {
			ImGui::Image(

				this->model.frameCollection[this->model.VideoSliderValue - 1].FeatureDrawOverlayTexture,
				ImVec2(1280, 720)
			);
		}
		else {
			ImGui::Image(

				this->model.frameCollection[this->model.VideoSliderValue - 1].FrameAsTexture,
				ImVec2(1280, 720)
			);
		}

        
		
		ImGui::Separator();

		ImGui::SliderInt(
			"Video", 
			&this->model.VideoSliderValue, 
			1, 
			this->model.frameCollection.size()
		);

    }


    // ToDo: Draw line to separate the toolbar



}