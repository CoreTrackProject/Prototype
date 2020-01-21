#pragma once

#include "BaseEditor/IEditorController.hpp"

#include "Viewport3DView.h"
#include "Viewport3DModel.h"

class Viewport3DController : public IEditorController {

private:
	Viewport3DView  view;
	Viewport3DModel model;

public:
	Viewport3DController();
	~Viewport3DController();

	void Init();
	void DeInit();
	void Render();
	
};