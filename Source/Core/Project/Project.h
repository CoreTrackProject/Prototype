#pragma once

#include "ProjectHeader.h"

#include <string>
#include <memory>
#include <mutex>

enum class ProjectStatus {
	ProjectNotSaved, // Not saved in terms of changes
	ProjectSaved, // Project has saved the changes or created on the filesystem
	ProjectNotSavedOnFilesystem, // New project created but not saved on the filesystem
	NoProjectLoaded // No project has been loaded (default project is used)
};

class Project {

private:
	std::string currProjectPath = "";
	std::shared_ptr<ProjectHeader> currProjectHeader;
	ProjectStatus status = ProjectStatus::NoProjectLoaded;
	std::mutex projectMutex;
	

public:
	Project();
	~Project();

	static Project &GetInstance();

	ProjectStatus GetProjectStatus();

	void OpenProject(std::string projectPath);
	
	void NewProject(std::string projectName);

	void SaveProject();
	
	void SaveProjectAs(std::string newProjectDirPath);
	
    void ImportFile(std::string filePath);

	std::shared_ptr<ProjectHeader> GetProjectHeader();

};
