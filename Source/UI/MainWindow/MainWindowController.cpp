#include "MainWindowController.h"
#include "Project/Project.h"

#include "Editor/TrackerEd/TrackerEdController.h"

#include <imgui.h>
#include <boost/log/trivial.hpp>


MainWindowController::MainWindowController() {}

MainWindowController::~MainWindowController() {}


void MainWindowController::Init() {
	this->view = std::make_shared<MainWindowView>(this->model);

	// Setup callbacks from view
	this->view->SetOpenProjectCallback(std::bind(&MainWindowController::OpenProjectCallback, this, std::placeholders::_1));
	this->view->SetNewProjectCallback(std::bind(&MainWindowController::NewProjectCallback, this, std::placeholders::_1));
	this->view->SetSaveProjectCallback(std::bind(&MainWindowController::SaveProjectCallback, this));
	this->view->SetSaveProjectAsCallback(std::bind(&MainWindowController::SaveProjectAsCallback, this, std::placeholders::_1));
    this->view->SetOpenNewTrackerEdInstanceCallback(std::bind(&MainWindowController::OpenNewTrackerEdInstanceCallback, this));
}

void MainWindowController::DeInit() {

}

void MainWindowController::Render() {
	this->view->Render();

    for(int i = 0; i < this->editorCollection.size(); i++) {
        this->editorCollection.at(i)->Render();
    }

}

void MainWindowController::OpenProjectCallback(std::string filePath) {

	// Where do the var checks happen? 
	// (View or Controller) for instance
	// if the file path is empty.

    BOOST_LOG_TRIVIAL(debug) << "OpenProjectCallback Result: " << filePath;
    Project::GetInstance().OpenProject(filePath);
}

void MainWindowController::NewProjectCallback(std::string newProjectPath) {
	BOOST_LOG_TRIVIAL(debug) << "NewProjectCallback";

    Project::GetInstance().NewProject(newProjectPath);
}

void MainWindowController::SaveProjectCallback() {
	BOOST_LOG_TRIVIAL(debug) << "SaveProjectCallback";
	Project::GetInstance().SaveProject();
}

void MainWindowController::SaveProjectAsCallback(std::string projectFilePath) {
    Project::GetInstance().SaveProjectAs(projectFilePath);
}

void MainWindowController::OpenNewTrackerEdInstanceCallback() {

    std::shared_ptr<TrackerEdController> ed = std::make_shared<TrackerEdController>();

    ed->Init();

    this->editorCollection.push_back(std::move(ed));
}
