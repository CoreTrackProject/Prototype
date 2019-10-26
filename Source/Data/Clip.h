#pragma once

#include <string>

class Clip {
	Clip(std::string clipName);
	~Clip();
	
	private:
		std::string clipName;
		
	public:
		std::string GetClipName();
};