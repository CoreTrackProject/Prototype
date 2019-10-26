#pragma once

#include "ProjectHeader.h"

#include <string>
#include <map>
#include <memory>

class Project2JSON {
	std::map<FileType, std::string> contentTypeEnumStr;

public:
	Project2JSON();
	~Project2JSON();

	std::string HeaderToJSON(ProjectHeaderData header);
	ProjectHeaderData JSONToHeader(std::string jsonStr);

	std::string ContentTypeToString(FileType type);
	FileType StringToContentType(std::string type);

};