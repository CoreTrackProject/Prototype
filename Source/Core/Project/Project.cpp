#include "Project.h"


#include "Task/ProjectIO/ProjectLoaderTask.h"
#include "Task/ProjectIO/ProjectSaveTask.h"

#include "Task/TaskScheduler.h"


#include <boost/log/trivial.hpp>


Project::Project() {

}

Project::~Project() {}


Project &Project::GetInstance() {
	static Project projectInstance;

	return projectInstance;
}

void Project::OpenProject(std::string projectFilePath) {
	std::shared_ptr<ProjectLoaderTask> newTask = std::make_shared<ProjectLoaderTask>();
    newTask->SetProjectFilePath(projectFilePath);

	TaskScheduler::GetInstance().AddTask(newTask, true);

	if (newTask->GetStatus() == TaskStatus::Finished) {
		this->status = ProjectStatus::ProjectSaved;

		this->currProject = std::static_pointer_cast<ProjectHeader>(newTask->GetResult());

		BOOST_LOG_TRIVIAL(info) << "Project opened: " << this->currProject->GetProjectHeaderData().ProjectName;

	}
	
}

void Project::NewProject(std::string projectNamePath) {
	ProjectHeaderData data;

    boost::filesystem::path path {projectNamePath};
    data.ProjectName = path.stem().string();


	this->currProject = std::make_shared<ProjectHeader>(data);
    this->currProject->SetProjectPath(projectNamePath);


    this->SaveProjectAs(projectNamePath);

    this->status = ProjectStatus::ProjectSaved;

    BOOST_LOG_TRIVIAL(info) << "New project \"" << projectNamePath << "\" created.";
}

ProjectStatus Project::GetProjectStatus() {
	return this->status;
}

void Project::SaveProject() {

	std::shared_ptr<ProjectSaveTask> newTask = std::make_shared<ProjectSaveTask>();

	newTask->SetProjectName(this->currProject->GetProjectHeaderData().ProjectName);
	newTask->SetProjectPath(this->currProject->GetProjectPath());
	newTask->SetProjectHeaderData(this->currProject->GetProjectHeaderData());

	newTask->Start();

	if (newTask->GetStatus() == TaskStatus::Finished) {
		this->status = ProjectStatus::ProjectSaved;
	}
}

void Project::SaveProjectAs(std::string newProjectDirPath) {
	std::shared_ptr<ProjectSaveTask> newTask = std::make_shared<ProjectSaveTask>();

	newTask->SetProjectName(this->currProject->GetProjectHeaderData().ProjectName);
	newTask->SetProjectPath(newProjectDirPath);
	newTask->SetProjectHeaderData(this->currProject->GetProjectHeaderData());

	newTask->Start();

	if (newTask->GetStatus() == TaskStatus::Finished) {
		this->status = ProjectStatus::ProjectSaved;
	}
}
