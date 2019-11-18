#include "TrackerEdView.h"
#include "Project/Project.h"

#include <Utils/File.h>

#include <stb_image.h>

TrackerEdView::TrackerEdView() {}

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


void TrackerEdView::drawToolbar() {

    if(ImGui::Button("+", ImVec2(32, 32))) {
        std::string filePath = "";
        if(File::PickSingleFile(filePath)) {
            Project::GetInstance().ImportFile(filePath);
        }
    }









    ImGui::SameLine();

    ImGui::NewLine();
    ImGui::Separator();
    // ToDo: Draw line to separate the toolbar



}
