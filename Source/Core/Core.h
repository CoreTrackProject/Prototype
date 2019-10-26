#pragma once

#include "WindowManager.h"

class Core {

private:
	WindowManager windowManager;

public:
	Core();
	~Core();
		
	public:
		void Run();
};