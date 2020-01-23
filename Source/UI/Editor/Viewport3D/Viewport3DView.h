#pragma once

#include "BaseEditor/IEditorView.hpp"

#include "Viewport3DModel.h"

#include <gl/glew.h>

class Viewport3DView : public IEditorView {
private:
	Viewport3DModel &model;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
	bool isOpen = true;

	


	public:
	Viewport3DView(Viewport3DModel &model);
	~Viewport3DView();

	 void Init();
	 void Render();
	 void DeInit();

private:
	void setViewportSize(int width, int height);
	
};