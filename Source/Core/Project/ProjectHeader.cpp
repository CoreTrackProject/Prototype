#include "ProjectHeader.h"

ProjectHeader::ProjectHeader() {}

ProjectHeader::ProjectHeader(ProjectHeaderData data) : data(data) {}

ProjectHeader::~ProjectHeader() {}

std::string ProjectHeader::GetProjectPath() {
	return this->projectPath;
}

void ProjectHeader::SetProjectPath(std::string projectPath) {
	this->projectPath = projectPath;
}

ProjectHeaderData ProjectHeader::GetProjectHeaderData() {
	return this->data;
}

void ProjectHeader::SetProjectHeaderData(ProjectHeaderData data) {
	this->data = data;
}