#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <GLFW/glfw3.h>

#include "../include/init.h"
#include "../include/camera.h"
#include "../include/graphics_setup.h"

struct UserPointerObjects {

	SwapChainInfo* swapChainInfo;
	UniformBufferObject* ubo;
	CameraHelper* cameraHelper;
};

void updateWindowTitle(GLFWwindow* window);
void updateFPS(GLFWwindow* window);
void initWindow(VulkanContext& context, SwapChainInfo& swapChainInfo, Camera& camera, UniformBufferObject& ubo, CameraHelper& cameraHelper);
void setCursorPositionCallback(GLFWwindow* window, double xPosition, double yPosition);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
void setScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

