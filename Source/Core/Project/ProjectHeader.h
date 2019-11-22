#pragma once

#include <string>
#include <vector>

#include <mutex>

enum class FileType {
	Image,
	Video,
	CacheFile,
	TrackerCollection,
	NodeComposition,
	Animation,
	Other
};

struct Content {
	std::string FilePath; // File path in the project directory structure
	std::string FileHash; // Hash of the file to check for intance if file has changed or cache is still valid
	FileType Type;
};

struct ProjectHeaderData {
	std::string ProjectName;
	std::vector<Content> ContentCollection;
};

class ProjectHeader {

private:
	std::string projectPath;
	ProjectHeaderData data;
	
	std::mutex projectHeaderMutex;

public:
	ProjectHeader();
	ProjectHeader(ProjectHeaderData data);

	~ProjectHeader();

	std::string GetProjectPath();
	void SetProjectPath(std::string projectPath);

	ProjectHeaderData GetProjectHeaderData();
	void SetProjectHeaderData(ProjectHeaderData data);

};
