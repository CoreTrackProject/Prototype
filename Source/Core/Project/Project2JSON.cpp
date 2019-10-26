#include "Project2JSON.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


#include <boost/uuid/uuid_io.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>




Project2JSON::Project2JSON() {

	contentTypeEnumStr.insert(std::make_pair(FileType::Animation, "Animation"));
	contentTypeEnumStr.insert(std::make_pair(FileType::CacheFile, "CacheFile"));
	contentTypeEnumStr.insert(std::make_pair(FileType::Image, "Image"));
	contentTypeEnumStr.insert(std::make_pair(FileType::NodeComposition, "NodeComposition"));
	contentTypeEnumStr.insert(std::make_pair(FileType::Other, "Other"));
	contentTypeEnumStr.insert(std::make_pair(FileType::TrackerCollection, "TrackerCollection"));
	contentTypeEnumStr.insert(std::make_pair(FileType::Video, "Video"));

}

Project2JSON::~Project2JSON() {}


std::string Project2JSON::HeaderToJSON(ProjectHeaderData header) {

	{
		using namespace boost::property_tree;

		ptree fullData;
		{
			ptree project;
			{
				ptree projectSub;
				{
					projectSub.put("ProjectName", header.ProjectName);
				}
				project.add_child("Project", projectSub);
			}
			fullData.add_child("Project", project);


			ptree content;
			{
				ptree contentSub;
				{
					for (Content cont : header.ContentCollection)
					{
						contentSub.put(cont.FileHash, cont.FilePath);
						contentSub.put("FileType", this->ContentTypeToString(cont.Type));
					}
				}
				content.add_child("Content", contentSub);
			}
			fullData.add_child("Content", content);
		}
		
		std::stringstream ss;
		json_parser::write_json(ss, fullData);

		return ss.str();
	}
}

ProjectHeaderData Project2JSON::JSONToHeader(std::string jsonStr) {
	
	{
		using namespace boost::property_tree;


		try {

			std::stringstream ss;
			ss << jsonStr;

			ptree project;
			json_parser::read_json(ss, project);


			ProjectHeaderData data;
			{
				data.ProjectName = project.get<std::string>("Project.Name", "");

				// FileCollection is a array  of uuid's which point to a file
				for (ptree::value_type &content : project.get_child("FileCollection"))
				{
					// Foreach file read the data
					std::string uuid = content.second.data();

					// Read the header data of a single file
					Content contentInfo;
					{
						contentInfo.FilePath = project.get<std::string>("Content." + uuid + ".FilePath", "");
						contentInfo.FileHash = project.get<std::string>("Content." + uuid + ".FileHash", "");
						contentInfo.Type =
							this->StringToContentType(project.get<std::string>("Content." + uuid + ".FileType", ""));

						contentInfo.UUID = boost::lexical_cast<boost::uuids::uuid>(uuid);

						// Missing is Cache and CacheHash

					}
					data.ContentCollection.push_back(contentInfo);
				}
			}

			return data;


		} catch (std::exception &ex) {
			BOOST_LOG_TRIVIAL(error) << ex.what();
		}


	}
}

std::string Project2JSON::ContentTypeToString(FileType type) {
	return this->contentTypeEnumStr.at(type);
}

FileType Project2JSON::StringToContentType(std::string type) {
	for (auto &i : this->contentTypeEnumStr) {
		if (i.second == type) {
			return i.first;
		}
	}
}
