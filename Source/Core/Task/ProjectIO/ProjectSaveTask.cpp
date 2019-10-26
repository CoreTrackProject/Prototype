#include "ProjectSaveTask.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>

#include "Project/Project2JSON.h"


ProjectSaveTask::ProjectSaveTask() {}

ProjectSaveTask::~ProjectSaveTask() {}


void ProjectSaveTask::Start() {
	this->status = TaskStatus::Running;

	if (!this->checkIfProjectExists()) {
		this->generateProjectDirStructure();
	}

	this->writeProjectHeader();

	this->status = TaskStatus::Finished;
}

void ProjectSaveTask::Stop() {
	this->status = TaskStatus::Stopped;
}

void ProjectSaveTask::Pause() {
	this->status = TaskStatus::Paused;
}

TaskStatus ProjectSaveTask::GetStatus() {
	return this->status;
}

std::shared_ptr<void> ProjectSaveTask::GetResult() {
	return nullptr;
}

void ProjectSaveTask::SetProjectPath(std::string projectDirPath) {
	this->projectDirPath = projectDirPath;
}

void ProjectSaveTask::SetProjectHeaderData(ProjectHeaderData headerData) {
	this->headerData = headerData;
}

void ProjectSaveTask::SetProjectName(std::string projectName) {
	this->projectName = projectName;
}

void ProjectSaveTask::writeProjectHeader() {
	
	{
		if (this->projectName.empty()) { return; }

		try {
			Project2JSON p2j;
			std::string hdrJSON = p2j.HeaderToJSON(this->headerData);


			boost::filesystem::path projectPath{ this->projectDirPath };
			projectPath.append("Project.json");

			if (!boost::filesystem::exists(projectPath))
			{
				std::ofstream out(projectPath.string());
				out << hdrJSON;
				out.close();

			} else {
				boost::filesystem::path tmpFileName = boost::filesystem::path(projectPath.filename().string() + "_tmp");
				boost::filesystem::path dirPath     = this->projectDirPath;
				boost::filesystem::path tmpFullPath = dirPath.append(tmpFileName.string());
				
				std::ofstream out(tmpFullPath.string());
				out << hdrJSON;
				out.close();

				boost::filesystem::rename(tmpFullPath, projectPath);
			}


		} catch (boost::exception &ex) {
			BOOST_LOG_TRIVIAL(fatal) << boost::diagnostic_information(ex);
			this->status = TaskStatus::Errored;
		}


	}

}

void ProjectSaveTask::generateProjectDirStructure() {
	// Create directories like Content, Cache etc.
	

}

bool ProjectSaveTask::checkIfProjectExists() {
	boost::filesystem::path projectPath{ this->projectDirPath };
	projectPath.append("Project.json");

	return boost::filesystem::exists(projectPath);
}