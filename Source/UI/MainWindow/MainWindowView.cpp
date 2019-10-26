#include "MainWindowView.h"

#include "Project/Project.h"

#include <nfd.h>
#include <boost/log/trivial.hpp>


MainWindowView::MainWindowView(MainWindowModel &mainWndModel) : model(mainWndModel) {

}

MainWindowView::~MainWindowView() {

}

void MainWindowView::Init() {

#ifndef FORCE_DEFAULT_STYLE
	// Setup Style
	// I took that nice dark color scheme from Derydoca as a base:
	// https://github.com/ocornut/imgui/issues/707?ts=4#issuecomment-463758243
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().GrabRounding = 0.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;
	ImGui::GetStyle().TabRounding = 0.0f;
	ImGui::GetStyle().FrameBorderSize = 2.f;
	ImGui::GetStyle().ScrollbarSize = 18.f;

	ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 0.9f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.007843f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] =
		ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] =
		ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] =
		ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	// TODO see docking branch of ImGui
	//  colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f,
	//  0.781f); colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f,
	//  0.180f, 0.9f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] =
		ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] =
		ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
#endif

}

void MainWindowView::Render() {

#ifdef _DEBUG
	ImGui::ShowDemoWindow();
#endif

	this->drawToolbar();
	this->drawNewProjectDialog();

}

void MainWindowView::DeInit() {

}

void MainWindowView::SetOpenProjectCallback(std::function<void(std::string)> openProjectCallback) {
	this->openProjectCallback = openProjectCallback;
}

void MainWindowView::SetNewProjectCallback(std::function<void(std::string)> newProjectCallback) {
	this->newProjectCallback = newProjectCallback;
}

void MainWindowView::SetSaveProjectCallback(std::function<void()> saveProjectCallback) {
	this->saveProjectCallback = saveProjectCallback;
}

void MainWindowView::SetSaveProjectAsCallback(std::function<void(std::string)> saveProjectAsCallback) {
	this->saveProjectAsCallback = saveProjectAsCallback;
}



void MainWindowView::drawToolbar() {
	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Project")) {
				// Clear input text
				memset(&this->newProjectInputText[0], 0, sizeof(this->newProjectInputText));
				this->showNewProjectDialog = true;
			}

			if (ImGui::MenuItem("Open Project..")) {
				this->openProject();
			}
			if (ImGui::MenuItem("Import..")) {
			
			}
			if (ImGui::MenuItem("Export..")) {
			
			}
			if (Project::GetInstance().GetProjectStatus() != ProjectStatus::NoProjectLoaded) {
				if (ImGui::MenuItem("Save..")) {
					this->saveProject();
				}
				if (ImGui::MenuItem("Save As..")) {
					this->saveProjectAs();
				}
			}

			ImGui::Separator();
			
			if (ImGui::MenuItem("Settings")) {
			
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {
			
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
			
			}  // Disabled item
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Project"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Editor"))
		{
			if (ImGui::MenuItem("Tracker Editor", "")) {
			
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::MenuItem("Github page", "")) {

			}
			ImGui::EndMenu();
		}



		ImGui::EndMainMenuBar();
	}
}

void MainWindowView::drawNewProjectDialog() {

	if (this->showNewProjectDialog) {

		ImGui::Begin("New project name");

		ImGui::PushItemWidth(ImGui::GetWindowWidth());

		bool const isReturnPressed = ImGui::InputText("", this->newProjectInputText, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank);
		bool const isSavePressed = ImGui::Button("Save");
		ImGui::SameLine(0.0f, 10.0f);
		bool const isCancelPressed = ImGui::Button("Cancel");

		if (isReturnPressed || isSavePressed) {

			if (strcmp(this->newProjectInputText, "") == 0) { return; }

			this->showNewProjectDialog = false;
			this->newProject(this->newProjectInputText);

		}
		else if (isCancelPressed) {
			this->showNewProjectDialog = false;

		}

		ImGui::End();

	}

}

void MainWindowView::openProject() {

	
	nfdchar_t  *outDirPath = NULL;
	nfdresult_t result = NFD_PickFolder("", &outDirPath);

	if (result == NFD_OKAY) {
		BOOST_LOG_TRIVIAL(info) << "Project directory selected: " << outDirPath;

		// Can a view class open a project or only the controller? -> controller
		// Loading a project is a task, can a view open a new task? -> NO!
		this->openProjectCallback(outDirPath);

		free(outDirPath);

	} else if (result == NFD_CANCEL) {
		BOOST_LOG_TRIVIAL(info) << "Project selection has been canceled";

	} else {
		BOOST_LOG_TRIVIAL(info) << "NFD Error: " << NFD_GetError();
	}
}

void MainWindowView::newProject(std::string newProjectName) {
	this->newProjectCallback(newProjectName);
}

void MainWindowView::saveProject() {

	// Check if project has been already been created on the filesystem

	auto status = Project::GetInstance().GetProjectStatus();
	if (status == ProjectStatus::ProjectNotSaved) {
		this->saveProjectCallback();

	} else if (status == ProjectStatus::ProjectNotSavedOnFilesystem) {
		this->saveProjectAs();
	}

}

void MainWindowView::saveProjectAs() {

	nfdchar_t  *outDirPath = NULL;
	nfdresult_t result = NFD_PickFolder("", &outDirPath);

	if (result == NFD_OKAY) {
		// Save project as callback
		this->saveProjectAsCallback(outDirPath);
		free(outDirPath);

	} else if (result == NFD_CANCEL) {
		BOOST_LOG_TRIVIAL(info) << "Directory selection has been canceled";

	} else {
		BOOST_LOG_TRIVIAL(info) << "NFD Error: " << NFD_GetError();
	}
}