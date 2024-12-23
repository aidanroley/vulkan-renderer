#pragma once

#include "../include/camera.h"
#include "../include/init.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// Forward decs for init.h
struct Vertex;
struct VulkanSetup;
struct SwapChainInfo;
struct UniformData;

struct UniformBufferObject {

    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct CameraHelper {

    Camera camera;

    CameraHelper(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
        : camera(position, target, up) {}
};

struct VertexData {

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct GraphicsSetup {

    UniformBufferObject* ubo;
    CameraHelper* cameraHelper;
    VertexData* vertexData;

    GraphicsSetup(UniformBufferObject* uboT, CameraHelper* ch, VertexData* vd)
        : ubo(uboT), cameraHelper(ch), vertexData(vd) {}
};

void initGraphics(GraphicsSetup& graphics, VulkanSetup& setup);
void initUBO(GraphicsSetup& graphics, SwapChainInfo& swapChainInfo, UniformData& uniformData, uint32_t currentImage);
void loadModel(VertexData& vertexData);
void updateUniformBuffers(GraphicsSetup& graphics, SwapChainInfo& swapChainInfo, UniformData& uniformData, uint32_t currentImage);