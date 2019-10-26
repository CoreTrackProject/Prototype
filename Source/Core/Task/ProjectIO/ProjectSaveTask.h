#pragma once

#include "Task/ITask.hpp"
#include "Project/ProjectHeader.h"

#include <string>
#include <boost/filesystem.hpp>

class ProjectSaveTask : public ITask {

	std::string projectDirPath;
	ProjectHeaderData headerData;
	std::string projectName;
	TaskStatus status = TaskStatus::Created;

private:
	void writeProjectHeader();
	void generateProjectDirStructure();
	bool checkIfProjectExists();

public:
	ProjectSaveTask();
	~ProjectSaveTask();

	void SetProjectHeaderData(ProjectHeaderData headerData);
	void SetProjectPath(std::string projectPath);
	void SetProjectName(std::string projectName);

	void Start();
	void Stop();
	void Pause();

	TaskStatus GetStatus();
	std::shared_ptr<void> GetResult();


	
};