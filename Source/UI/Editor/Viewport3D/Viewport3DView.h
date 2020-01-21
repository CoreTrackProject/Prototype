#pragma once

#include "BaseEditor/IEditorView.hpp"

#include "Viewport3DModel.h"

#include <gl/glew.h>

class Viewport3DView : public IEditorView {
private:
	Viewport3DModel &model;

	GLuint FramebufferName = 0;
	GLuint renderedTexture;
	GLuint depthrenderbuffer;
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

	public:
	Viewport3DView(Viewport3DModel &model);
	~Viewport3DView();

	 void Init();
	 void Render();
	 void DeInit();
	
};