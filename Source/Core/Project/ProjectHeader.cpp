#include "ProjectHeader.h"

ProjectHeader::ProjectHeader() {}

ProjectHeader::ProjectHeader(ProjectHeaderData data) : data(data) {}

ProjectHeader::~ProjectHeader() {}

std::string ProjectHeader::GetProjectPath() {
	return this->projectPath;
}

void ProjectHeader::SetProjectPath(std::string projectPath) {
	if (!this->projectHeaderMutex.try_lock()) {
		return;
	}
	this->projectPath = projectPath;
	this->projectHeaderMutex.unlock();
}

ProjectHeaderData ProjectHeader::GetProjectHeaderData() {
	return this->data;
}

void ProjectHeader::SetProjectHeaderData(ProjectHeaderData data) {
	if (!this->projectHeaderMutex.try_lock()) {
		return;
	}
	this->data = data;
	this->projectHeaderMutex.unlock();
}