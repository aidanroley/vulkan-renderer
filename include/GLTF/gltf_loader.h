#pragma once
#include "vk_types.h"
#include "Engine/vk_helper_funcs.h"
#include "Descriptor/vk_descriptor.h"
#include "Engine/pbr_pipeline.h"

// forward decs
class VkEngine;
struct VertexData;
std::optional<AllocatedImage> loadImage(VkEngine* engine, fastgltf::Asset& asset, fastgltf::Image& image);

struct Node;
class gltfData {
public:

	gltfData() = default;

	std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshStorage;
	std::unordered_map<std::string, std::shared_ptr<Node>> nodeStorage;
	std::unordered_map<std::string, AllocatedImage> imageStorage;
	std::unordered_map<std::string, std::shared_ptr<gltfMaterial>> materialStorage;

	// nodes that dont have a parent, for iterating through the file in tree order
	std::vector<std::shared_ptr<Node>> topNodes;
	AllocatedBuffer materialDataBuffer;

	void drawNodes(DrawContext& ctx);

	//~gltfData() { destroyAll(); };

private:

	//VulkanContext* context;
	struct EnginePassed {

		VkDevice device;
		AllocatedImage whiteImage;
		VkSampler defaultSamplerLinear;
	};

	EnginePassed passed = {};

	//void destroyAll();
};

struct Node {

	std::weak_ptr<Node> parent;
	std::vector<std::shared_ptr<Node>> children; // this is a shared pointer because if there are no references to a child it can be removed

	glm::mat4 localTransform;
	glm::mat4 worldTransform;

	std::string name;
	// this makes it so only MeshNode draws actually do anything,, otherwise just a recursive call for node structure.
	virtual void Draw(DrawContext& ctx)
	{
		// draw children (pre order transversal)
		for (auto& c : children) {
			c->Draw(ctx);
		}
	}

};


struct MeshNode : public Node {

	std::shared_ptr<MeshAsset> mesh;
	RenderObject createRenderObject(const GeoSurface& surface);
	virtual void Draw(DrawContext& ctx) override;
};

std::shared_ptr<gltfData> loadGltf(VkEngine* engine, std::filesystem::path path);

VkFilter extract_filter(fastgltf::Filter filter);
VkSamplerMipmapMode extract_mipmap_mode(fastgltf::Filter filter);