#pragma once

#include "BaseEditor/IEditorController.hpp"
#include "MainWindowView.h"
#include "MainWindowModel.h"

#include <memory>
#include <vector>

class MainWindowController : public IEditorController {

private:
	std::shared_ptr<MainWindowView> view;
	MainWindowModel model;

    std::vector<std::shared_ptr<IEditorController>> editorCollection;

public:
	MainWindowController();
	~MainWindowController();

	void Init();
	void DeInit();

	void Render();

	void OpenProjectCallback(std::string filePath);
	void NewProjectCallback(std::string newProjectName);
	void SaveProjectCallback();
    void SaveProjectAsCallback(std::string projectFilePath);
    void OpenNewTrackerEdInstanceCallback();
	void OpenNewViewport3DInstanceCallback();
};
