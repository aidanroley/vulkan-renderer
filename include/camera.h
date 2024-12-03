#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Euler based camera system (pitch and yaw, no roll)
class Camera {

	const float CAMERA_YAW = -90.0f;
	const float CAMERA_PITCH = 0.0f;
	const float MOUSE_X_INIT = 400.0f; // center of screen
	const float MOUSE_Y_INIT = 300.0f;

public:

	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;

	float yaw;
	float pitch;

	double currentMouseX;
	double currentMouseY;
	
	Camera(glm::vec3 startPosition, glm::vec3 startTarget) : cameraPos(startPosition), cameraTarget(startTarget), cameraDirection(glm::vec3(0.0f, 0.0f, -1.0f)), 
		   yaw(CAMERA_YAW), pitch(CAMERA_PITCH), currentMouseX(MOUSE_X_INIT), currentMouseY(MOUSE_Y_INIT) {

		updateDirection();
	}
	
	glm::mat4 getViewMatrix() const {

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		return glm::lookAt(cameraPos, cameraTarget, up);
	}

	glm::vec3 getCameraDirection() const {

		return cameraDirection;
	}

	void processMouseInput(double xpos, double ypos) {

		float xOffset = static_cast<float>(xpos) - currentMouseX;
		float yOffset = static_cast<float>(ypos) - currentMouseY;

		const float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		updateDirection();

		currentMouseX = xpos;
		currentMouseY = ypos;

	}

private:

	void updateDirection() {

		cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection.y = sin(glm::radians(pitch));
		cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	}

};

#endif