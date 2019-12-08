#include "TrackerEdView.h"
#include "Project/Project.h"
#include "Utils/FileUtils.h"

#include <stb_image.h>
#include <GL/glew.h>

#include <imgui_internal.h>

#include <boost/log/trivial.hpp>

TrackerEdView::TrackerEdView(TrackerEdModel &model) : model(model) {}

TrackerEdView::~TrackerEdView() {}


void TrackerEdView::Init() {


}

void TrackerEdView::Render(){
    ImGui::Begin("Tracker Editor");

    this->drawToolbar();
    this->drawBody();

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

void TrackerEdView::SetAddTrackerCallback(std::function<void ()> addTrackerCallback) {
    this->addTrackerCallback = addTrackerCallback;
}


void TrackerEdView::drawToolbar() {


    if(this->currState == TrackerEdState::AddTracker) {

        // Escape key to leave tracker selection mode
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            this->currState = TrackerEdState::Ready;
            return;
        }


        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

        ImGui::Text("Press ESC to leave tracker selection mode");

    }

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


    if(this->model.CurrVideo.HasFrames()) {

        ImGui::SameLine();

        if (ImGui::Button("[ ]", ImVec2(32, 32))) {
            this->currState = TrackerEdState::AddTracker;

            return;
            //this->addTrackerCallback();
        }
    }

    if(this->currState == TrackerEdState::AddTracker) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }


    ImGui::Separator();

}

void TrackerEdView::drawBody() {

    if (this->model.CurrVideo.HasFrames()) {

        ImGui::BeginChildFrame(1, ImVec2(1280, 720));


        ImVec2 pos = ImGui::GetCursorScreenPos();
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging() && this->currState == TrackerEdState::Ready) {
            this->offset.x += ImGui::GetIO().MouseDelta.x;
            this->offset.y += ImGui::GetIO().MouseDelta.y;
        }

        ImGui::GetWindowDrawList()->AddImage(
            this->model.CurrVideo.GetTextureFrameByIdx(this->model.VideoSliderValue - 1),
            ImVec2(pos.x + this->offset.x, pos.y + this->offset.y),
            ImVec2(pos.x + this->offset.x + 1280, pos.y + this->offset.y + 720)
            );

        // Draw track markers
        for(TrackMarker marker : this->model.TrackMarkerCollection) {

            ImVec2 markerPos = ImVec2(pos.x + this->offset.x + marker.x,
                                      pos.y + this->offset.y + marker.y);


            ImGui::GetWindowDrawList()->AddCircle(markerPos, 3, IM_COL32(255, 255, 255, 255));
            ImGui::GetWindowDrawList()->AddRect(
                ImVec2(markerPos.x - 50, markerPos.y - 50),
                ImVec2(markerPos.x + 50, markerPos.y + 50),
                IM_COL32(255, 255, 255, 255));
        }

        ImGui::EndChildFrame();






        // Tracker add logic
        {
            if(this->currState == TrackerEdState::AddTracker) {
                ImGuiIO *io = &ImGui::GetIO();

                ImVec2 mPos = ImGui::GetMousePos();

                if(io->MouseReleased[0]) {

                    if(mPos.x  >= pos.x &&
                        mPos.y >= pos.y &&
                        mPos.x <= (pos.x + 1280) &&
                        mPos.y <= (pos.y + 720)) {


                        mPos.x -= (offset.x + pos.x);
                        mPos.y -= (offset.y + pos.y);

                        this->addTrackMarker(mPos.x, mPos.y);

                        return;
                    }
                }
            }
        }


        ImGui::Separator();



        ImGui::SliderInt(
            "Video",
            &this->model.VideoSliderValue,
            1,
            this->model.CurrVideo.GetFrameCount()
            );

    }
}

void TrackerEdView::addTrackMarker(int x, int y) {
    // Add tracker marker by position

    BOOST_LOG_TRIVIAL(debug) << "Add track marker (" << x << ", " <<  y << ")";


    TrackMarker newMarker;
    newMarker.width  = 100;
    newMarker.height = 100;
    newMarker.x      = x;
    newMarker.y      = y;
    newMarker.Selected = true;


    this->model.TrackMarkerCollection.push_back(newMarker);



}
