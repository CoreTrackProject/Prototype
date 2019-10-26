#pragma once

#include "Task/ITask.hpp"
#include "Project/ProjectHeader.h"

#include <string>
#include <boost/filesystem.hpp>

class ProjectLoaderTask : public ITask {

	std::string projectDirPath;
	std::shared_ptr<ProjectHeader> header;
	TaskStatus status;

private:
	void readProjectHeader();
	bool containsProjectJSONFile();
	boost::filesystem::path getProjectFilePath();

public:
	ProjectLoaderTask();
	~ProjectLoaderTask();

	void Start();
	void Stop();
	void Pause();
	TaskStatus GetStatus();
	std::shared_ptr<void> GetResult();

	void SetProjectPath(std::string projectPath);

	
};