#pragma once

#include "BaseEditor/IEditorView.hpp"
#include "MainWindowModel.h"



#include <imgui.h>
#include <functional>
#include <string>

class MainWindowView : IEditorView {
private:
	MainWindowModel &model;

	std::function<void(std::string)> openProjectCallback;
	std::function<void(std::string)> newProjectCallback;
	std::function<void()> saveProjectCallback;
	std::function<void(std::string)> saveProjectAsCallback;

	char newProjectInputText[256] = "";

public:
	MainWindowView(MainWindowModel &model);
	~MainWindowView();

	void Init();
	void Render();
	void DeInit();

	void SetOpenProjectCallback(std::function<void(std::string)> openProjectCallback);
	void SetNewProjectCallback(std::function<void(std::string)> newProjectCallback);
	void SetSaveProjectCallback(std::function<void()> saveProjectCallback);
	void SetSaveProjectAsCallback(std::function<void(std::string)> saveProjectAsCallback);

private:
    void drawToolbar();

	void openProject();
    void newProject();

	void saveProject();
	void saveProjectAs();

};
