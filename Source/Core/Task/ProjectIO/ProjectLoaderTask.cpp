#include "ProjectLoaderTask.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>

#include "Project/Project2JSON.h"

ProjectLoaderTask::ProjectLoaderTask() {}

ProjectLoaderTask::~ProjectLoaderTask() {}


void ProjectLoaderTask::Start() {
	this->status = TaskStatus::Running;

	if (!this->projectDirPath.empty() && this->containsProjectJSONFile()) {
		this->readProjectHeader();
	}
	else {
		this->status = TaskStatus::Errored;
		return;
	}

	this->status = TaskStatus::Finished;
}

void ProjectLoaderTask::Stop() {
	this->status = TaskStatus::Stopped;
}

void ProjectLoaderTask::Pause() {
	this->status = TaskStatus::Paused;
}

TaskStatus ProjectLoaderTask::GetStatus() {
	return this->status;
}

std::shared_ptr<void> ProjectLoaderTask::GetResult() {
	return this->header;
}

void ProjectLoaderTask::SetProjectPath(std::string projectDirPath) {
	this->projectDirPath = projectDirPath;
}

void ProjectLoaderTask::readProjectHeader() {
	
	{
		using namespace boost::property_tree;


		try {
			Project2JSON p2j;


			std::ifstream input(this->getProjectFilePath().c_str());

			std::stringstream sstr;
			while (input >> sstr.rdbuf());

			
			ProjectHeaderData data = p2j.JSONToHeader(sstr.str());
			std::shared_ptr<ProjectHeader> hdr = std::make_shared<ProjectHeader>();
			hdr->SetProjectHeaderData(data);
			hdr->SetProjectPath(this->projectDirPath);

			this->header.swap(hdr);

			
		} catch (boost::exception &ex) {
			BOOST_LOG_TRIVIAL(fatal) << boost::diagnostic_information(ex);
			this->status = TaskStatus::Errored;
		}

	}

}

bool ProjectLoaderTask::containsProjectJSONFile() {

	bool prjFileExists = boost::filesystem::exists(this->getProjectFilePath());

	if (!prjFileExists)
	{
		BOOST_LOG_TRIVIAL(error) << "Project.json has not been found in the directory: " << this->projectDirPath;
	}

	return prjFileExists;
}

boost::filesystem::path ProjectLoaderTask::getProjectFilePath() {
	boost::filesystem::path prjPath(this->projectDirPath);
	prjPath.append("Project.json");

	return prjPath;
}