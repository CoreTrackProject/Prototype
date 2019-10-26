#pragma once

#include <MainWindow/MainWindowController.h>

struct ImVec4;
struct GLFWwindow;


class WindowManager {

	GLFWwindow *mainWindow = nullptr;
	MainWindowController mainWndController;


public:
	static void LogError(int error, const char *description);

	static void KeyCallback(GLFWwindow *window, int key, int val, int action, int mods);
	static void CharCallback(GLFWwindow *window, unsigned int c);
	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow *window, double mouse_x, double mouse_y);

public:
	WindowManager();
	~WindowManager();

	void Init();
	void BeginMessageLoop();
	void DeInit();

	void OnKeyCallback(GLFWwindow *window, int key, int val, int action, int mods);
	void OnCharCallback(GLFWwindow *window, unsigned int c);
	void OnMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	void OnCursorPosCallback(GLFWwindow *window, double mouse_x, double mouse_y);


private:
	void glNewFrame(GLFWwindow *window, ImVec4 clear_color, int *display_w,int *display_h);
	void glEndFrame(GLFWwindow *window);

};