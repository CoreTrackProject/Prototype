#include "Core.h"

Core::Core() {}
Core::~Core() {}

void Core::Run() {
	this->windowManager.Init();
	this->windowManager.BeginMessageLoop();
	this->windowManager.DeInit();
}