#include "Project.h"
#include "Utils/File.h"


#include "Task/ProjectIO/ProjectLoaderTask.h"
#include "Task/ProjectIO/ProjectSaveTask.h"

#include "Task/TaskScheduler.h"


#include <boost/log/trivial.hpp>



Project::Project() {
    // Init dummy project

    this->currProjectHeader = std::make_shared<ProjectHeader>();

}

Project::~Project() {}


Project &Project::GetInstance() {
	static Project projectInstance;
	return projectInstance;
}


void Project::OpenProject(std::string projectFilePath) {

	if (!this->projectMutex.try_lock()) {
		return;
	}

	std::shared_ptr<ProjectLoaderTask> newTask = std::make_shared<ProjectLoaderTask>();
    newTask->SetProjectFilePath(projectFilePath);

	TaskScheduler::GetInstance().AddTask(newTask, true);

	if (newTask->GetStatus() == TaskStatus::Finished) {
		this->status = ProjectStatus::ProjectSaved;

		this->currProjectHeader = std::static_pointer_cast<ProjectHeader>(newTask->GetResult());

		BOOST_LOG_TRIVIAL(info) << "Project opened: " << this->currProjectHeader->GetProjectHeaderData().ProjectName;

	}

	this->projectMutex.unlock();
	
}

void Project::NewProject(std::string projectNamePath) {

	if (!this->projectMutex.try_lock()) {
		return;
	}

	ProjectHeaderData data;

    boost::filesystem::path path {projectNamePath};
    data.ProjectName = path.stem().string();


	this->currProjectHeader = std::make_shared<ProjectHeader>(data);
    this->currProjectHeader->SetProjectPath(projectNamePath);


    this->SaveProjectAs(projectNamePath);

    this->status = ProjectStatus::ProjectSaved;

    BOOST_LOG_TRIVIAL(info) << "New project \"" << projectNamePath << "\" created.";

	this->projectMutex.unlock();
}

ProjectStatus Project::GetProjectStatus() {
	return this->status;
}

void Project::SaveProject() {

	if (!this->projectMutex.try_lock()) {
		return;
	}

	std::shared_ptr<ProjectSaveTask> newTask = std::make_shared<ProjectSaveTask>();

	newTask->SetProjectName(this->currProjectHeader->GetProjectHeaderData().ProjectName);
	newTask->SetProjectPath(this->currProjectHeader->GetProjectPath());
	newTask->SetProjectHeaderData(this->currProjectHeader->GetProjectHeaderData());

	newTask->Start();

	if (newTask->GetStatus() == TaskStatus::Finished) {
		this->status = ProjectStatus::ProjectSaved;
	}

	this->projectMutex.unlock();
}

void Project::SaveProjectAs(std::string newProjectDirPath) {

	if (!this->projectMutex.try_lock()) {
		return;
	}

	std::shared_ptr<ProjectSaveTask> newTask = std::make_shared<ProjectSaveTask>();

	newTask->SetProjectName(this->currProjectHeader->GetProjectHeaderData().ProjectName);
	newTask->SetProjectPath(newProjectDirPath);
	newTask->SetProjectHeaderData(this->currProjectHeader->GetProjectHeaderData());

	newTask->Start();

	if (newTask->GetStatus() == TaskStatus::Finished) {
		this->status = ProjectStatus::ProjectSaved;
	}

	this->projectMutex.unlock();
}



void Project::ImportFile(std::string filePath) {

	if (!this->projectMutex.try_lock()) {
		return;
	}
    
	// TODO: Create a new Task to import and cache file and remove this method

   Content newFile;
   newFile.Type     = FileType::Video;
   newFile.FileHash = File::CalcFileHash(filePath);
   newFile.FilePath = filePath;

   BOOST_LOG_TRIVIAL(info) << "Imported file: " << filePath;
   BOOST_LOG_TRIVIAL(info) << "File hash: " << newFile.FileHash;

   this->currProjectHeader->GetProjectHeaderData().ContentCollection.push_back(std::move(newFile));

   this->projectMutex.unlock();
}

std::shared_ptr<ProjectHeader> Project::GetProjectHeader() {
	return this->currProjectHeader;
}
