#include "Viewport3DView.h"

#include <imgui.h>




Viewport3DView::Viewport3DView(Viewport3DModel &model) : model(model) {
	this->Init();
}

Viewport3DView::~Viewport3DView() {
	this->DeInit();
}

void Viewport3DView::Init() {

	// framebuffer configuration
    // -------------------------

	 // framebuffer configuration
	// -------------------------
	
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it


	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {


	}
		//cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Viewport3DView::Render() {

	ImGui::Begin("3D Viewport", &this->isOpen, ImGuiWindowFlags_NoScrollbar);

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	//ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));
	



	static int textureWidth = 0;  // = vMax.x - vMin.x;
	static int textureHeight = 0; // = vMax.y - vMin.y;

	int currWidth  = vMax.x - vMin.x;
	int currHeight = vMax.y - vMin.y;

	if (textureWidth != currWidth || 
		textureHeight != currHeight) {

		textureWidth  = vMax.x - vMin.x;
		textureHeight = vMax.y - vMin.y;

		this->setViewportSize(textureWidth, textureHeight);
	}

	// =====================
	// = Draw viewport     =
	// =====================


	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//ImGui::ImageButton((void*)textureColorbuffer, ImVec2(textureWidth, textureHeight));
	ImGui::Image((void*)textureColorbuffer, ImVec2(textureWidth, textureHeight));
	
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRect(ImVec2(vMin.x, vMin.y), ImVec2(vMax.x, vMax.y), IM_COL32(255, 255, 0, 255));

	ImGui::End();
}

void Viewport3DView::DeInit() {

}

void Viewport3DView::setViewportSize(int width, int height) {

	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	

}