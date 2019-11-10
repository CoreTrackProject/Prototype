#include "WindowManager.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "roboto_mono_embed.inc.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw_gl3.h>

#include <boost/log/trivial.hpp>


WindowManager::WindowManager() {}
WindowManager::~WindowManager() {}


void WindowManager::LogError(int errorcode, const char *description) {
	BOOST_LOG_TRIVIAL(error) << "Error code: " << errorcode << " Description: " << description;
}

void WindowManager::KeyCallback(GLFWwindow *window, int key, int val, int action, int mods) {
	static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->OnKeyCallback(window, key, val, action, mods);
}

void WindowManager::CharCallback(GLFWwindow *window, unsigned int c) {
	static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->OnCharCallback(window, c);
}

void WindowManager::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->OnMouseButtonCallback(window, button, action, mods);
}

void WindowManager::CursorPosCallback(GLFWwindow *window, double mouse_x, double mouse_y) {
	static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->OnCursorPosCallback(window, mouse_x, mouse_y);
}


void WindowManager::Init() {

	// Init GLFW
	{

		if (!glfwInit()) {
			BOOST_LOG_TRIVIAL(error) << "Failed to initialize GLFW3";
		}

		glfwSetErrorCallback(WindowManager::LogError);


		this->mainWindow =
			glfwCreateWindow(1920, 1080, "CoreTrack Prototype", nullptr, nullptr);

		glfwMakeContextCurrent(this->mainWindow);

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


		glfwSetWindowUserPointer(this->mainWindow, this);

		// Register MainWindow events
		glfwSetKeyCallback(this->mainWindow, KeyCallback);
		glfwSetCharCallback(this->mainWindow, CharCallback);
		glfwSetMouseButtonCallback(this->mainWindow, MouseButtonCallback);
		glfwSetCursorPosCallback(this->mainWindow, CursorPosCallback);

	}
	
	// Init gl3w
	{
		if (gl3wInit() != 0) {
			BOOST_LOG_TRIVIAL(info) << "Failed to create OpenGL3 context.";
			exit(EXIT_FAILURE);
		}
	}

	// Init ImGui
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

		ImGui_ImplGlfwGL3_Init(this->mainWindow, true);
	}

	this->mainWndController.Init();
}


void WindowManager::OnKeyCallback(GLFWwindow *window, int key, int val, int action, int mods) {
	//BOOST_LOG_TRIVIAL(debug) << "Key: " << key << " Val: " << val << " Action: " << action << " Mods: " << mods << std::endl;
}

void WindowManager::OnCharCallback(GLFWwindow *window, unsigned int c) {
	//BOOST_LOG_TRIVIAL(debug) << "Char: " << c << std::endl;
}

void WindowManager::OnMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	//BOOST_LOG_TRIVIAL(debug) << " Button: " << button << " Action: " << action << " Mods: " << mods << std::endl;
}

void WindowManager::OnCursorPosCallback(GLFWwindow *window, double mouse_x, double mouse_y) {
	//BOOST_LOG_TRIVIAL(debug) << " Mouse X: " << mouse_x << " Mouse Y: " << mouse_y << std::endl;
}



void WindowManager::BeginMessageLoop() {

	ImVec4 background_color = ImVec4(0.05f, 0.05f, 0.08f, 1.00f);
	int display_w, display_h;

	while (!glfwWindowShouldClose(this->mainWindow)) {
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		this->glNewFrame(this->mainWindow, background_color, &display_w, &display_h);

		// call each editor for gui rendering here
		this->mainWndController.Render();


		this->glEndFrame(this->mainWindow);
	}
}

void WindowManager::DeInit() {

	this->mainWndController.DeInit();

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(this->mainWindow);
	glfwTerminate();
}

void WindowManager::glNewFrame(GLFWwindow *window, ImVec4 clear_color, int *display_w, int *display_h) {
	
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

void WindowManager::glEndFrame(GLFWwindow *window) {
	glUseProgram(0);


	ImGui::Render();


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
		sprintf(title, "FPS [%dFPS]", frameCount);
		glfwSetWindowTitle(window, title);
		frameCount = 0;
		previousTime = currentTime;
	}
}