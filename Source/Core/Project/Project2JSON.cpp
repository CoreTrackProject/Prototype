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
                project.put("Name", header.ProjectName);


                // Add file list array
                ptree contentCollection;
                {
                    for (Content cont : header.ContentCollection)
                    {
                        ptree item;
                        item.put("", cont.FileHash);
                        contentCollection.push_back(std::make_pair("", item));
                    }

                }
                project.add_child("ContentCollection", contentCollection);


			}
			fullData.add_child("Project", project);


			ptree content;
            {
                for (Content cont : header.ContentCollection)
                {
                    ptree contentDesc;

                    content.put("FilePath", cont.FilePath);
                    content.put("FileType", this->ContentTypeToString(cont.Type));

                    content.add_child(cont.FileHash, content);
                }
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
                for (ptree::value_type &content : project.get_child("Project.ContentCollection"))
				{
					// Foreach file read the data
                    std::string fileHash = content.second.data();

					// Read the header data of a single file
					Content contentInfo;
					{
                        contentInfo.FilePath = project.get<std::string>("Content." + fileHash + ".FilePath", "");
                        contentInfo.FileHash = project.get<std::string>("Content." + fileHash + ".FileHash", "");
						contentInfo.Type =
                            this->StringToContentType(project.get<std::string>("Content." + fileHash + ".FileType", ""));


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

    return ProjectHeaderData();
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
    return FileType::Other;
}
