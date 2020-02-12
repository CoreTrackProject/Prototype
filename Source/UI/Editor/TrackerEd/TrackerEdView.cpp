#include "TrackerEdView.h"

#include "Utils/FileUtils.h"

#include <stb_image.h>
#include <GL/glew.h>

#include <imgui_internal.h>

#include <boost/log/trivial.hpp>

#include <cmath>

#include <ImSequencer.h>

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

void TrackerEdView::SetAddTrackMarkerCallback(std::function<void(TrackMarker)> addTrackMarkerCallback) {
	this->addTrackMarkerCallback = addTrackMarkerCallback;
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

		float edWindowWidth = ImGui::GetWindowWidth();

        ImGui::BeginChildFrame(1, ImVec2(edWindowWidth, 720));
        ImVec2 panPanelPos = ImGui::GetCursorScreenPos();

        if (ImGui::IsItemActive() && ImGui::IsMouseDragging() && this->currState == TrackerEdState::Ready) {
            this->imgViewPos.x += ImGui::GetIO().MouseDelta.x;
            this->imgViewPos.y += ImGui::GetIO().MouseDelta.y;
        }

        ImGui::GetWindowDrawList()->AddImage(
            this->model.CurrVideo.GetTextureFrameByIdx(this->model.VideoSliderValue),
            ImVec2(panPanelPos.x + this->imgViewPos.x, panPanelPos.y + this->imgViewPos.y),
            ImVec2(panPanelPos.x + this->imgViewPos.x + this->model.CurrVideo.GetVideoDim().width, panPanelPos.y + this->imgViewPos.y + this->model.CurrVideo.GetVideoDim().height)
            );

        // Draw track markers
		for (TrackSequence::TrackSequenceItem sequenceItem : this->model.sequence.myItems) {

			// The video clip is the first sequence item, so do not try to draw the marker
			if (sequenceItem.SequenceItemType != TrackSequenceItemType::Track) { continue; }

			TrackMarker marker = sequenceItem.marker;

            int currFrameIdx = this->model.VideoSliderValue;

            if(marker.BeginFrame <= currFrameIdx &&
                marker.EndFrame  >= currFrameIdx) {



                // Display tracked marker
                //
                //-----------O------------  // Video Frames (O = Cursor of curr video frame)
                //xxxxxxx--------xxxxxxxxx  // TrackMarker
                //       ^      ^
                // Begin Frame   End Frame
                // Right Marker = MarkerCollection[CurrFrameIdx - Begin Frame]

				// Draw check if it is reference marker (placed by the user) or tracked by libmv
				
				TrackMarker currMarker = marker;
				if (this->model.VideoSliderValue != marker.BeginFrame) {
					currMarker.mvMarker = marker.TrackedMarker[this->model.VideoSliderValue - marker.BeginFrame];
				}
				

                int markerIdx = (this->model.VideoSliderValue) - marker.BeginFrame;

                // Offset of the image view
                ImVec2 offset = ImVec2(
                    panPanelPos.x + this->imgViewPos.x,
                    panPanelPos.y + this->imgViewPos.y);

                // Top left corner of a trackmarker
                ImVec2 markerTLCornerPosAbsolute = ImVec2(offset.x + currMarker.GetSearchArea().tl().x, offset.y + currMarker.GetSearchArea().tl().y);
				// Down right corner of  a trackmarker
				ImVec2 markerDRCornerPosAbsolute = ImVec2(offset.x + currMarker.GetSearchArea().br().x, offset.y + currMarker.GetSearchArea().br().y);

				// Draw the search area of a trackmarker
                ImGui::GetWindowDrawList()->AddRect(
                    markerTLCornerPosAbsolute,
					markerDRCornerPosAbsolute,
                    IM_COL32(255, 255, 0, 255) // yellow
                    );

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddCircle(markerDRCornerPosAbsolute,       6.0f, IM_COL32(0, 0, 0, 255));
				draw_list->AddCircleFilled(markerDRCornerPosAbsolute, 5.0f, IM_COL32(255, 255, 0, 255));
				draw_list->AddCircle(markerDRCornerPosAbsolute,       1.0f, IM_COL32(0, 0, 0, 255));
				
				// Draw search area of a trackmarker
				mv::Quad2Df trackAreaQuad = currMarker.GetMvTrackArea();
				ImGui::GetWindowDrawList()->AddQuad(
					ImVec2(offset.x + trackAreaQuad.coordinates(0, 0), offset.y + trackAreaQuad.coordinates(0, 1)),
					ImVec2(offset.x + trackAreaQuad.coordinates(1, 0), offset.y + trackAreaQuad.coordinates(1, 1)),
					ImVec2(offset.x + trackAreaQuad.coordinates(2, 0), offset.y + trackAreaQuad.coordinates(2, 1)),
					ImVec2(offset.x + trackAreaQuad.coordinates(3, 0), offset.y + trackAreaQuad.coordinates(3, 1)),
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
                        mPos.x <= (panPanelPos.x + edWindowWidth) &&
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

		// ------------------------------
		// Draw the sequencer
		// ------------------------------

		//ImGui::Begin("Sequencer");

		ImGui::PushItemWidth(100);
		ImGui::InputInt("Current Frame", &this->model.VideoSliderValue);
		ImGui::PopItemWidth();
		ImGui::Separator();

		ImSequencer::Sequencer(
			&this->model.sequence,
			&this->model.VideoSliderValue,
			&this->model.SequencerExpandet, 
			&this->model.SequencerSelectedEntry, 
			&this->model.FirstFrame,
			ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME
		);



    }
}

void TrackerEdView::addTrackMarker(int x, int y) {
    // Add tracker marker by position



    BOOST_LOG_TRIVIAL(debug) << "Add track marker (" << x << ", " <<  y << ")";


    TrackMarker newMarker = TrackMarker(cv::Point2d(x, y));

    newMarker.BeginFrame = this->model.VideoSliderValue; // First frame index is 0, GUI shows 1
    newMarker.EndFrame   = this->model.VideoSliderValue;

	this->addTrackMarkerCallback(newMarker);

}

