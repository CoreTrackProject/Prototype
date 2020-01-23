#include "TrackerEdView.h"
#include "Project/Project.h"
#include "Utils/FileUtils.h"

#include <stb_image.h>
#include <GL/glew.h>

#include <imgui_internal.h>

#include <boost/log/trivial.hpp>

#include <cmath>

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

void TrackerEdView::SetTrackerMarkersCallback(std::function<void ()> trackMarkersCallback) {
    this->trackMarkersCallback = trackMarkersCallback;
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

        ImGui::SameLine();

        if (ImGui::Button(">>", ImVec2(32, 32))) {
            this->trackMarkersCallback();
            return;
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
        ImVec2 panPanelPos = ImGui::GetCursorScreenPos();

        if (ImGui::IsItemActive() && ImGui::IsMouseDragging() && this->currState == TrackerEdState::Ready) {
            this->imgViewPos.x += ImGui::GetIO().MouseDelta.x;
            this->imgViewPos.y += ImGui::GetIO().MouseDelta.y;
        }

        ImGui::GetWindowDrawList()->AddImage(
            this->model.CurrVideo.GetTextureFrameByIdx(this->model.VideoSliderValue - 1),
            ImVec2(panPanelPos.x + this->imgViewPos.x, panPanelPos.y + this->imgViewPos.y),
            ImVec2(panPanelPos.x + this->imgViewPos.x + 1280, panPanelPos.y + this->imgViewPos.y + 720)
            );


        // Draw track markers
        for(TrackMarker marker : this->model.TrackMarkerCollection) {

            int currFrameIdx = this->model.VideoSliderValue - 1;

            if(marker.BeginFrame <= currFrameIdx &&
                marker.EndFrame  >= currFrameIdx) {


                // Display tracked marker
                //
                //-----------O------------  // Video Frames (O = Cursor of curr video frame)
                //xxxxxxx--------xxxxxxxxx  // TrackMarker
                //       ^      ^
                // Begin Frame   End Frame
                // Right Marker = MarkerCollection[CurrFrameIdx - Begin Frame]


                TrackMarker currMarker;
                int markerIdx = (this->model.VideoSliderValue - 1) - marker.BeginFrame;

                if(marker.EndFrame > 0 && marker.Anim.size() > markerIdx) {
                    currMarker = marker.Anim.at(markerIdx);
                } else {
                    currMarker = marker;
                }

                // Offset of the image view
                ImVec2 offset = ImVec2(
                    panPanelPos.x + this->imgViewPos.x,
                    panPanelPos.y + this->imgViewPos.y);

                // Top left corner of a trackmarker
                ImVec2 markerTLCornerPosAbsolute = ImVec2(offset.x + currMarker.GetSearchArea().tl().x, offset.y + currMarker.GetSearchArea().tl().y);
				ImVec2 markerTDCornerPosAbsolute = ImVec2(offset.x + currMarker.GetSearchArea().br().x, offset.y + currMarker.GetSearchArea().br().y);

                ImGui::GetWindowDrawList()->AddRect(
                    markerTLCornerPosAbsolute,
					markerTDCornerPosAbsolute,
                    IM_COL32(255, 255, 0, 255) // yellow
                    );


				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddCircle(markerTDCornerPosAbsolute, 6.0f, IM_COL32(0, 0, 0, 255));
				draw_list->AddCircleFilled(markerTDCornerPosAbsolute, 5.0f, IM_COL32(255, 255, 0, 255));
				draw_list->AddCircle(markerTDCornerPosAbsolute, 1.0f, IM_COL32(0, 0, 0, 255));
				

                ImGui::GetWindowDrawList()->AddRect(
                    ImVec2(markerTLCornerPosAbsolute.x + currMarker.GetTrackArea().tl().x, markerTLCornerPosAbsolute.y + currMarker.GetTrackArea().tl().y),
                    ImVec2(markerTLCornerPosAbsolute.x + currMarker.GetTrackArea().br().x, markerTLCornerPosAbsolute.y + currMarker.GetTrackArea().br().y),
                    IM_COL32(0, 255, 255, 255)
                    );

            } // end if


        }

        ImGui::EndChildFrame();






        // Tracker add logic
        {
            if(this->currState == TrackerEdState::AddTracker) {
                ImGuiIO *io = &ImGui::GetIO();

                ImVec2 mPos = ImGui::GetMousePos();

                // Dragging
                if(io->MouseReleased[0]) {

                    if(mPos.x  >= panPanelPos.x &&
                        mPos.y >= panPanelPos.y &&
                        mPos.x <= (panPanelPos.x + 1280) &&
                        mPos.y <= (panPanelPos.y + 720)) {


                        mPos.x -= (imgViewPos.x + panPanelPos.x);
                        mPos.y -= (imgViewPos.y + panPanelPos.y);

                        this->addTrackMarker(mPos.x, mPos.y);

                        return;
                    }
                }

                // Zooming (TODO)
                if(io->MouseWheel != 0.0f) {
                    //BOOST_LOG_TRIVIAL(debug) << "Mouse wheel used: " << io->MouseWheel;
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


    //TrackMarker newMarker = TrackMarker(cv::Rect2f(x - 50, y - 50, 100, 100));

    //newMarker.BeginFrame = this->model.VideoSliderValue - 1; // First frame index is 0, GUI shows 1
    //newMarker.EndFrame   = this->model.VideoSliderValue - 1;

    // this->model.TrackMarkerCollection.push_back(newMarker);


	mv::Marker newMvMarker;
	newMvMarker.center = mv::Vec2f(x, y);

	newMvMarker.clip = 0;
	newMvMarker.reference_clip = 0;

	// This doesn't seem to be used at all by libmv TrackRegion
	newMvMarker.model_type = mv::Marker::POINT;
	newMvMarker.model_id   = 0;
	newMvMarker.track      = 0;



	mv::Quad2Df trackPattern;
	trackPattern.coordinates(0, 0) = x;
	trackPattern.coordinates(0, 1) = y;

	trackPattern.coordinates(1, 0) = x + 100;
	trackPattern.coordinates(1, 1) = y;

	trackPattern.coordinates(2, 0) = x + 100;
	trackPattern.coordinates(2, 1) = y + 100;

	trackPattern.coordinates(3, 0) = x;
	trackPattern.coordinates(3, 1) = y + 100;

	mv::Region sRegion;
	sRegion.min = mv::Vec2f(x - 50, y - 50);
	sRegion.max = mv::Vec2f(x + 50, y + 50);
	newMvMarker.search_region = std::move(sRegion);




	//newMvMarker.patch = trackPattern;

	newMvMarker.frame = this->model.VideoSliderValue - 1;
	newMvMarker.model_type = mv::Marker::ModelType::POINT;



	newMvMarker.weight = 1.0;
	


}

