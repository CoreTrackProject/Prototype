#include "MainWindowController.h"
#include "Project/Project.h"

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
}

void MainWindowController::DeInit() {

}

void MainWindowController::Render() {
	this->view->Render();
}

void MainWindowController::OpenProjectCallback(std::string directoryPath) {

	// Where do the var checks happen? 
	// (View or Controller) for instance
	// if the file path is empty.

	BOOST_LOG_TRIVIAL(debug) << "OpenProjectCallback Result: " << directoryPath;
	Project::GetInstance().OpenProject(directoryPath);
}

void MainWindowController::NewProjectCallback(std::string newProjectName) {
	BOOST_LOG_TRIVIAL(debug) << "NewProjectCallback";

	Project::GetInstance().NewProject(newProjectName);
}

void MainWindowController::SaveProjectCallback() {
	BOOST_LOG_TRIVIAL(debug) << "SaveProjectCallback";
	Project::GetInstance().SaveProject();
}

void MainWindowController::SaveProjectAsCallback(std::string projectDirPath) {
	Project::GetInstance().SaveProjectAs(projectDirPath);
}