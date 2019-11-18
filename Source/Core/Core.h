#pragma once

#include "WindowManager.h"
#include "Project/Project.h"
#include "Task/TaskScheduler.h"


class Core {

private:
	WindowManager windowManager;


public:
	Core();
    ~Core();

    void Run();

};
