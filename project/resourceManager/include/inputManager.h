#ifndef INPUT_MANAGER_H_
#define INPUT_MANAGER_H_

#include <GLFW/glfw3.h>

struct mousePoint {
	float x;
	float y;
};

class inputManager {
private:
	// mousePos actually stores the "last" mouse position
	static mousePoint mousePos;
	static mousePoint mouseOffset;
	static bool firstMouse;

	static bool wKey;
	static bool sKey;
	static bool aKey;
	static bool dKey;
	static bool spaceKey;
	static bool lShiftKey;
	static bool escapeKey;

public:
	void resetOffset() { 
		mouseOffset.x = 0;
		mouseOffset.y = 0;
	}

	mousePoint getMouse() { return mousePos; }
	float getMouseX() { return mousePos.x; }
	float getMouseY() { return mousePos.y; }
	mousePoint getOffset() { return mouseOffset; }
	float getOffsetX() { return mouseOffset.x; }
	float getOffsetY() { return mouseOffset.y; }

	bool getWKey() { return wKey; }
	bool getSKey() { return sKey; }
	bool getAKey() { return aKey; }
	bool getDKey() { return dKey; }
	bool getSpaceKey() { return spaceKey; }
	bool getLShiftKey() { return lShiftKey; }
	bool getEscapeKey() { return escapeKey; }

	static void processKeyboardInput(GLFWwindow* window);
	static void processMouseInput(GLFWwindow* window, double xpos, double ypos);

private:

};

#endif // INPUT_MANAGER_H_