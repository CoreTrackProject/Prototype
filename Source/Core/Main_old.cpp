#include <iostream>

#include "GL/gl3w.h"
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw_gl3.h>
#include <imgui_node_editor.h>

#include "roboto_mono_embed.inc.h"

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <thread>
#include <vector>

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//ImTextureID imageTexture = nullptr;
//ImVec2 imageTextureSize  = ImVec2(0, 0);
ImVec2 imageBeginPos     = ImVec2(0, 0);


struct CVideo {

private:
	cv::VideoCapture video;
	std::vector<cv::Mat> frameCollection;
	std::vector<ImTextureID> frameImTextureCollection;
	ImVec2 videoDim;

	void generateOGLTexture(cv::Mat videoFrame) {
		GLuint tmpTexture;

		// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glGenTextures(1, &tmpTexture);
		glBindTexture(GL_TEXTURE_2D, tmpTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Set texture clamping method
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAM_TO_EDGE);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		cv::cvtColor(videoFrame, videoFrame, CV_RGB2BGR);

		glTexImage2D(
			GL_TEXTURE_2D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,            // Internal colour format to convert to
			videoFrame.cols,   // Image width  i.e. 640 for Kinect in standard mode
			videoFrame.rows,   // Image height i.e. 480 for Kinect in standard mode
			0,                 // Border width in pixels (can either be 1 or 0)
			GL_RGB,            // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE,  // Image data type
			videoFrame.ptr()); // The actual image data itself

		frameImTextureCollection.emplace_back((ImTextureID)tmpTexture);

	}

public:
	void loadVideo(std::string path) {
		video = cv::VideoCapture(path);
		
		this->videoDim = ImVec2(
			video.get(CV_CAP_PROP_FRAME_WIDTH), 
			video.get(CV_CAP_PROP_FRAME_HEIGHT)
		);
		
		int videoFrameCount = int(video.get(cv::CAP_PROP_FRAME_COUNT));
		video.set(cv::CAP_PROP_POS_FRAMES, 0);
		
		frameCollection.reserve(videoFrameCount);
		frameImTextureCollection.reserve(videoFrameCount);

		cv::Mat videoFrame;
		while (video.read(videoFrame)) {
			frameCollection.emplace_back(videoFrame);
			generateOGLTexture(videoFrame);
		}
	}

	cv::Mat getVideoFrameByIdx(int idx) {
		if (this->frameCollection.size() <= 0) return cv::Mat();

		return this->frameCollection.at(idx);
	}

	ImTextureID getImTextureByIdx(int idx) {
		if (this->frameImTextureCollection.size() <= 0) return ImTextureID();
		return this->frameImTextureCollection.at(idx);
	}

	int getFrameCount() {
		return this->frameCollection.size();
	}

	ImVec2 getVideoDim() {
		return this->videoDim;
	}

};

static int currFrameIdx = 0;
static CVideo video;

struct CTracker2D {
private:
	ImVec2 trackerPos        = ImVec2(0,   0);
	ImVec2 trackerDim        = ImVec2(100, 100);
	bool trackerSelected     = false;
	ImVec4 trackingRectColor = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
	
public:
	ImVec2 getTrackerPos() {
		return this->trackerPos;
	}
	void setTrackerPos(ImVec2 trackerPos) {
		this->trackerPos = trackerPos;
	}
	ImVec2 getTrackerDim() {
		return this->trackerDim;
	}
	void setTrackerDim(ImVec2 trackerDim) {
		this->trackerDim = trackerDim;
	}
	bool getTrackerSelected() {
		return this->trackerSelected;
	}
	void setTrackerSelected(bool selected) {
		this->trackerSelected = selected;
	}
	void setTrackingRectColor(ImVec4 trackingRectColor) {
		this->trackingRectColor = trackingRectColor;
	}
	ImVec4 getTrackingRectColor() {
		return this->trackingRectColor;
	}
	cv::Rect getOCVRect() {
		return cv::Rect(
			trackerPos.x - (trackerDim.x / 2),
			trackerPos.y - (trackerDim.y / 2),
			trackerDim.x,
			trackerDim.y
		);
	}
};

// Position on image ex. 0 - 1920 (max image dimensions) relative to screenpos
static CTracker2D tracker2d;



struct CTracker2DAnim {
	std::vector<CTracker2D> trackerAnim;

	std::vector<CTracker2D> getTrackerAnim() {
		return this->trackerAnim;
	}

	void setTrackerAnim(std::vector<CTracker2D> trackerAnim) {
		this->trackerAnim = trackerAnim;
	}
};

static CTracker2DAnim tracker2dAnim;



static bool trackerRunning = false;



static void key_callback(GLFWwindow *window, int key, int, int action,
                         int mods) {
	
	ImGuiIO &io = ImGui::GetIO();
	if (action == GLFW_PRESS) io.KeysDown[key] = true;
	if (action == GLFW_RELEASE) io.KeysDown[key] = false;
	
	std::cout << char(key) << "\n";
	
	(void)mods;  // Modifiers are not reliable across systems
	io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT]   || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT]     || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER]   || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

}

static void char_callback(GLFWwindow *, unsigned int c) {
	ImGuiIO &io = ImGui::GetIO();
	if (c > 0 && c < 0x10000) {
		io.AddInputCharacter(static_cast<unsigned short>(c));
	}
}

static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int mods) {}

static void cursor_pos_callback(GLFWwindow *window, double mouse_x,
                                double mouse_y) {}

static void gui_new_frame() {
  glfwPollEvents();
  // ImGui_ImplOpenGL3_NewFrame();
  // ImGui_ImplGlfw_NewFrame();
  // ImGui::NewFrame();
  ImGui_ImplGlfwGL3_NewFrame();
}

static void gl_new_frame(GLFWwindow *window, ImVec4 clear_color, int *display_w,
                         int *display_h) {
  // Rendering
  glfwGetFramebufferSize(window, display_w, display_h);
  glViewport(0, 0, *display_w, *display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
}

static void gl_gui_end_frame(GLFWwindow *window) {
  glUseProgram(0);

  ImGui::Render();
  // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window);
  glFlush();

  static int frameCount = 0;
  static double currentTime = glfwGetTime();
  static double previousTime = currentTime;
  static char title[256];

  frameCount++;
  currentTime = glfwGetTime();
  const auto deltaTime = currentTime - previousTime;
  if (deltaTime >= 1.0) {
    sprintf(title, "nnview GUI [%dFPS]", frameCount);
    glfwSetWindowTitle(window, title);
    frameCount = 0;
    previousTime = currentTime;
  }
}

static void error_callback(int error, const char *description) {
  std::cerr << "GLFW Error : " << error << ", " << description << std::endl;
}

void trackFootageAsync()
{
	// 1. Get First frame
	// 2. Cut out tracker frame
	// 3. Track on next frame
	// 4. Store 2D movement in vector

	if (video.getFrameCount() <= 0) {
		return;
	}


	


	//BindCVMat2GLTexture(video.getImTextureByIdx);

	// Calc image region
	cv::Rect2d roi = tracker2d.getOCVRect();

	cv::Ptr<cv::Tracker> tracker = cv::TrackerCSRT::create();
	tracker->init(video.getVideoFrameByIdx(0), roi);

	tracker2dAnim.getTrackerAnim().clear();

	for (int i = 0; i < video.getFrameCount(); i++) {

		currFrameIdx = i;
		// Update the tracking result
		bool ok = tracker->update(video.getVideoFrameByIdx(i), roi);

		// Calc center, new dim of rectangle

		video.getVideoFrameByIdx(i);
		

		std::vector<CTracker2D> tracker2D = tracker2dAnim.getTrackerAnim();
		tracker2D.push_back(tracker2d);


		if (ok) {
			std::cout << "Tracking ok" << std::endl;

		} else {
			std::cout << "Tracking failed" << std::endl;

		}

	}
	


	




	trackerRunning = false;
}

void draw_viewport() {
	
	ImGui::ShowDemoWindow();

	ImGui::Begin("Video Viewport", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::Button("Load Video")) {
		video.loadVideo("C:/Users/rphil/Desktop/Prototype/Content/GH010431.MP4");
	}

	if (ImGui::Button("Start tracking")) {


		trackerRunning = true;
		std::thread t1(trackFootageAsync);
		t1.detach();

	}

	ImGui::BeginChildFrame(1, ImGui::GetContentRegionAvail());
	imageBeginPos = ImGui::GetCursorScreenPos();
	ImGui::Image(video.getImTextureByIdx(currFrameIdx), ImVec2(1920, 1080));

	ImGui::SliderInt("", &currFrameIdx, 0, video.getFrameCount() - 1);
		
	// Tracker rectangle ui logic
	{
		// Offset between mouse cursor and the tracking rectangle
		static ImVec2 trackerPosOffsetMouse = ImVec2(0, 0);


		ImVec2 mousePos  = ImGui::GetIO().MousePos;
		
		static bool offsetMouseTrackerCenterCalculated = false;
		static bool offsetMouseScaleCtrlCalculated = false;

		ImVec2 rectUpLeftCorner = ImVec2(
			imageBeginPos.x + tracker2d.getTrackerPos().x - (tracker2d.getTrackerDim().x / 2),
			imageBeginPos.y + tracker2d.getTrackerPos().y - (tracker2d.getTrackerDim().y / 2)
		);

		ImVec2 rectDownRightCorner = ImVec2(
			imageBeginPos.x + tracker2d.getTrackerPos().x + (tracker2d.getTrackerDim().x / 2),
			imageBeginPos.y + tracker2d.getTrackerPos().y + (tracker2d.getTrackerDim().y / 2)
		);


		// Logic to drag the tracker inside the window
		if (ImGui::IsMouseDown(0))
		{

			// Check if mouse click is inside tracker rectangle
			if (mousePos.x >= rectUpLeftCorner.x    &&
				mousePos.y >= rectUpLeftCorner.y    &&
				mousePos.x <= rectDownRightCorner.x &&
				mousePos.y <= rectDownRightCorner.y
				) {
				
				tracker2d.setTrackerSelected(true);
				
				if (!offsetMouseTrackerCenterCalculated) {

					// Calc tracker offset
					trackerPosOffsetMouse = ImVec2(
						mousePos.x - (imageBeginPos.x + tracker2d.getTrackerPos().x),
						mousePos.y - (imageBeginPos.y + tracker2d.getTrackerPos().y)
					);

					offsetMouseTrackerCenterCalculated = true;
				}

				tracker2d.setTrackingRectColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

				// Tracker relative pos
				ImVec2 newTrackerPosRel = ImVec2(
					(mousePos.x - trackerPosOffsetMouse.x) - imageBeginPos.x,
					(mousePos.y - trackerPosOffsetMouse.y) - imageBeginPos.y
				);

				// Drag tracker rectangle here
				tracker2d.setTrackerPos(
					newTrackerPosRel
				);

			}

			bool downRightRectSelected = (
				rectDownRightCorner.x      <= mousePos.x && 
				rectDownRightCorner.y      <= mousePos.y && 
				rectDownRightCorner.x + 10 >= mousePos.x && 
				rectDownRightCorner.y + 10 >= mousePos.y);
			

			static ImVec2 deltaMousePos = ImVec2(0, 0);

			// Down right scale control clicked
			if (downRightRectSelected) {

				if (!offsetMouseScaleCtrlCalculated) {
					deltaMousePos = ImVec2(mousePos.x - rectDownRightCorner.x, mousePos.y - rectDownRightCorner.y);
					offsetMouseScaleCtrlCalculated = true;
				}

				// Delta between mouse pos and tracker scale control top left corner (also down right corner of tracker rectangle)

				// Min position to scale the tracker (tracker center cross)
				ImVec2 minPos = tracker2d.getTrackerPos();

				// Max position to scale the tracker (down right corner)
				ImVec2 maxPos = ImVec2(imageBeginPos.x + video.getVideoDim().x, imageBeginPos.y + video.getVideoDim().y);

				
				if (rectDownRightCorner.x > minPos.x &&
					rectDownRightCorner.y > minPos.y ||
					rectDownRightCorner.x < maxPos.x &&
					rectDownRightCorner.y < maxPos.y) {


					// Calc new scale of tracker rectangle
					ImVec2 deltaTrackerCenterAndTrackerDownRightCorner = ImVec2(
						((mousePos.x - deltaMousePos.x) - (imageBeginPos.x + tracker2d.getTrackerPos().x)) * 2,
						((mousePos.y - deltaMousePos.y) - (imageBeginPos.y + tracker2d.getTrackerPos().y)) * 2
					);

					tracker2d.setTrackerDim(
						deltaTrackerCenterAndTrackerDownRightCorner
					);

				}

				// TODO: Check if tracker not exceeds min or max

			}


		}

		if (ImGui::IsMouseReleased(0) && tracker2d.getTrackerSelected()) {
			tracker2d.setTrackingRectColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
			tracker2d.setTrackerSelected(false);
			offsetMouseTrackerCenterCalculated = false;
			offsetMouseScaleCtrlCalculated = false;
		}
		
		// Draw tracker rectangle
		{

			ImDrawCornerFlags corners_none = 0;
			ImDrawList *draw_list = ImGui::GetWindowDrawList();


			const ImU32 col = ImColor(tracker2d.getTrackingRectColor());
			ImVec2 trackerPos = tracker2d.getTrackerPos();
			ImVec2 trackerDim = tracker2d.getTrackerDim();

			draw_list->AddRect(
				ImVec2(
				(trackerPos.x + imageBeginPos.x)     - (trackerDim.x / 2),
					(trackerPos.y + imageBeginPos.y) - (trackerDim.y / 2)
				),
				ImVec2(
				(trackerPos.x + imageBeginPos.x)     + (trackerDim.x / 2),
					(trackerPos.y + imageBeginPos.y) + (trackerDim.y / 2)
				),
				col,
				0.0f,
				corners_none,
				0);

			// Draw cross in center of tracking rectangle
			draw_list->AddLine(
				ImVec2((imageBeginPos.x + trackerPos.x) - 10, imageBeginPos.y + trackerPos.y),
				ImVec2((imageBeginPos.x + trackerPos.x) + 10, imageBeginPos.y + trackerPos.y),
				col);

			draw_list->AddLine(
				ImVec2(imageBeginPos.x + trackerPos.x, (imageBeginPos.y + trackerPos.y) - 10),
				ImVec2(imageBeginPos.x + trackerPos.x, (imageBeginPos.y + trackerPos.y) + 10),
				col);


			draw_list->AddRect(
				ImVec2(
					rectDownRightCorner.x + 10,
					rectDownRightCorner.y + 10
				),
				ImVec2(
					rectDownRightCorner.x,
					rectDownRightCorner.y
				),
				col,
				0.0f,
				corners_none,
				0);

			
			std::string dim = std::to_string((int)tracker2d.getTrackerDim().x) + "/" + std::to_string((int)tracker2d.getTrackerDim().y);
			std::string pos = std::to_string((int)tracker2d.getTrackerPos().x) + "/" + std::to_string((int)tracker2d.getTrackerPos().y);
			
			draw_list->AddText(ImVec2(rectUpLeftCorner.x + 2, rectUpLeftCorner.y - 35), col, dim.c_str());
			draw_list->AddText(ImVec2(rectUpLeftCorner.x + 2, rectUpLeftCorner.y - 20), col, pos.c_str());

		}
	}


	ImGui::EndChildFrame();

	ImGui::End();
}

void loadTexture(std::string path) {
  // "C:/Users/rphil/Desktop/Prototype/Content/test.png"
  // Goal: Load this texture and present in imgui

  int width = 0, height = 0, component = 0;
  if (auto data = stbi_load(path.c_str(), &width, &height, &component, 4)) {
    GLuint my_opengl_texture;

    glGenTextures(1, &my_opengl_texture);
    glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    // auto texture = ImGui_CreateTexture(data, width, height);
    stbi_image_free(data);

    //imageTexture = (ImTextureID)my_opengl_texture;
    //imageTextureSize = ImVec2(width, height);

  } else {
    //imageTexture = nullptr;
    //imageTextureSize = ImVec2(0, 0);
  }
}



int main(int argc, char **argv) {
	GLFWwindow *window = nullptr;

	// Init GLFW3
	{
		if (!glfwInit()) {
			exit(EXIT_FAILURE);
		}

		glfwSetErrorCallback(error_callback);

		window =
			glfwCreateWindow(1920, 1080, "CoreTrack Prototype", nullptr, nullptr);
		glfwMakeContextCurrent(window);

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		glfwSetKeyCallback(window, key_callback);
		glfwSetCharCallback(window, char_callback);

		// glfwSetWindowUserPointer(window, &app);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_pos_callback);

		if (gl3wInit() != 0) {
			std::cerr << "Failed to create OpenGL3 context.\n";
			exit(EXIT_FAILURE);
		}
	}

	// Init imgui
	{
		ImGui::CreateContext();
		auto &io = ImGui::GetIO();

		const float default_font_scale = 16.f;
		ImFontConfig roboto_config;
		strcpy(roboto_config.Name, "Roboto");
		roboto_config.SizePixels = default_font_scale;
		roboto_config.OversampleH = 2;
		roboto_config.OversampleV = 2;

		io.Fonts->AddFontFromMemoryCompressedTTF(
			roboto_mono_compressed_data, roboto_mono_compressed_size,
			default_font_scale, &roboto_config);

		ImGui_ImplGlfwGL3_Init(window, true);

#ifndef FORCE_DEFAULT_STYLE
		// Setup Style
		// I took that nice dark color scheme from Derydoca as a base:
		// https://github.com/ocornut/imgui/issues/707?ts=4#issuecomment-463758243
		ImGui::StyleColorsDark();
		ImGui::GetStyle().WindowRounding = 1.0f;
		ImGui::GetStyle().ChildRounding = 1.0f;
		ImGui::GetStyle().FrameRounding = 1.0f;
		ImGui::GetStyle().GrabRounding = 1.0f;
		ImGui::GetStyle().PopupRounding = 1.0f;
		ImGui::GetStyle().ScrollbarRounding = 1.0f;
		ImGui::GetStyle().TabRounding = 1.0f;
		ImGui::GetStyle().FrameBorderSize = 2.f;
		ImGui::GetStyle().ScrollbarSize = 18.f;

		ImVec4 *colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 0.9f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.007843f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
		colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
		colors[ImGuiCol_ScrollbarGrabHovered] =
			ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
		colors[ImGuiCol_ScrollbarGrabActive] =
			ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
		colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
		colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
		colors[ImGuiCol_TabUnfocusedActive] =
			ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
		// TODO see docking branch of ImGui
		//  colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f,
		//  0.781f); colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f,
		//  0.180f, 0.9f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
		colors[ImGuiCol_PlotHistogramHovered] =
			ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_NavWindowingHighlight] =
			ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
#endif
	};

	ImVec4 background_color = ImVec4(0.05f, 0.05f, 0.08f, 1.00f);

	loadTexture("C:/Users/rphil/Desktop/Prototype/Content/test.png");

	while (!glfwWindowShouldClose(window)) {
		gui_new_frame();

		int display_w, display_h;
		gl_new_frame(window, background_color, &display_w, &display_h);

		draw_viewport();

		gl_gui_end_frame(window);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}