#pragma once
#include "../../precompile/pch.h"
#include "../../include/scene_info/gltf_loader.h"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include <cmath>

// This file will be a general GLTF loader once I get to the point where I don't have to hardcode anything for sun temple

const std::string SUN_TEMPLE_MODEL_PATH = "assets/SunTemple/SunTemple.glb";

void loadModel_SunTemple(VertexData& vertexData) {

    auto parser = fastgltf::Parser();
    auto data = fastgltf::GltfDataBuffer::FromPath(SUN_TEMPLE_MODEL_PATH);
    if (data.error() != fastgltf::Error::None) {

        std::cerr << "file load after fromPath failed ;_;";
    }

    // It's worth noting asset's type becomes fastgltf::Expected<class fastgltf::Asset> and is used in similar fashion to a smart pointer
    auto asset = parser.loadGltfBinary(data.get(), "assets/SunTemple", fastgltf::Options::None);

    if (asset.error() != fastgltf::Error::None) {

        std::ostringstream errorMessage;
        errorMessage << "Error: Failed to load glTF: " << fastgltf::getErrorMessage(asset.error());
        throw std::invalid_argument(errorMessage.str());
    }

    auto validationErrors = fastgltf::validate(asset.get());
    if (validationErrors != fastgltf::Error::None) {

        std::cout << "Validation Errors Found:" << std::endl;
    }
    else {

        std::cout << "The asset is valid!" << std::endl;
    }

        loadMeshData(asset.get(), vertexData);


        // SUN_TEMPLE SPECIFIC TRANSFORMS
        glm::mat3 flipMatrix = glm::mat3(1.0f);
        flipMatrix[1][1] = -1.0f;
        
        // Define the 90-degree rotation matrix for X-axis
        glm::mat3 rotationMatrix = glm::mat3(1.0f); 
        rotationMatrix[1][1] = 0.0f;  
        rotationMatrix[1][2] = -1.0f; 
        rotationMatrix[2][1] = 1.0f;  
        rotationMatrix[2][2] = 0.0f; 

        
        for (auto& vertex : vertexData.vertices) {
            vertex.pos = flipMatrix * rotationMatrix * vertex.pos;
        }

        // Next, remove duplicates from vertex buffer and change index buffer accordingly
        optimizeVertexBuffer(vertexData);
}

// This function uses a map to make sure every vertex in the buffer is unique while updating the index buffer accordingly
void optimizeVertexBuffer(VertexData& vertexData) {

    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;
    std::vector<Vertex> optimizedVertexBuffer;
    std::vector<uint32_t> updatedIndexBuffer;

    for (uint32_t index : vertexData.indices) {

        const Vertex& vertex = vertexData.vertices[index];
        if (uniqueVertices.find(vertex) == uniqueVertices.end()) {

            uniqueVertices[vertex] = static_cast<uint32_t>(optimizedVertexBuffer.size());
            optimizedVertexBuffer.push_back(vertex);
        }
        updatedIndexBuffer.push_back(uniqueVertices[vertex]);
    }
    vertexData.vertices = std::move(optimizedVertexBuffer);
    vertexData.indices = std::move(updatedIndexBuffer);

    // normalize texcoords
    for (auto& vertex : vertexData.vertices) {

        vertex.texCoord = glm::mod(vertex.texCoord + 1.0f, 1.0f);
    }

}

void loadMeshData(const fastgltf::Asset& asset, VertexData& vertexData) {

    const std::vector<fastgltf::Mesh>& meshes = asset.meshes;

    std::cout << "loading " + std::to_string(meshes.size()) + " meshes";
    for (std::size_t meshIdx = 0; meshIdx < meshes.size(); meshIdx++) {

        for (const fastgltf::Primitive& primitive : meshes[meshIdx].primitives) {

            if (!primitive.indicesAccessor.has_value()) {

                throw std::invalid_argument("error: gltf file needs indexed geometry");
            }
            loadMeshIndices(asset, primitive, vertexData);
            loadMeshVertices(asset, &primitive, vertexData);
        }
    }
}

void loadMeshVertices(const fastgltf::Asset& asset, const fastgltf::Primitive* primitive, VertexData& vertexData) {

    // fetch matIdx if it exists
    std::size_t materialIdx = -1;
    if (primitive->materialIndex.has_value()) materialIdx = primitive->materialIndex.value();

    // Puts the primitive data into this vector, then pushes each el in this vector into the main one once everything is done
    std::vector<Vertex> tempVertices{};

    auto* positionIterator = primitive->findAttribute("POSITION");
    if (!positionIterator) {

        throw std::invalid_argument("POSITION attribute not found.");
    }
    auto& posAccessor = asset.accessors[positionIterator->accessorIndex];

    constexpr float scaleFactor = 0.0006f;
    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        asset, posAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
            
            Vertex vert{};
            vert.pos = glm::vec3(pos.x(), pos.y(), pos.z()) * scaleFactor;
            tempVertices.push_back(vert);
        });
    
    auto* texIterator = primitive->findAttribute("TEXCOORD_0");

    if (!texIterator) {

        throw std::invalid_argument("COLOR attribute not found.");
    }
    auto& colorAccessor = asset.accessors[texIterator->accessorIndex];
    
    // get TEXCOORD data
    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
        asset, colorAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {

            glm::vec2 vertexTexCoord = glm::vec2(uv.x(), uv.y());
            tempVertices[idx].texCoord = vertexTexCoord;
        });

    for (Vertex& v : tempVertices) {

        if (materialIdx) {

            v.texIndex = materialIdx;
        }
        vertexData.vertices.push_back(v);
    }
}

void loadMeshIndices(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive, VertexData& vertexData) {

    // Indices must be offset by the size of vertex buffer at the beginning of this function since the indices in the
    // GLTF file are relative to the primitive, not all vertices
    int startingIdx = vertexData.vertices.size();

    std::vector<std::uint32_t> indices;
    if (primitive.indicesAccessor.has_value()) {

        auto& accessor = asset.accessors[primitive.indicesAccessor.value()];
        indices.resize(accessor.count);

        fastgltf::iterateAccessor<std::uint32_t>(asset, accessor, [&](std::uint32_t index) {

            vertexData.indices.push_back(index + startingIdx);
        });
    }

}

// Helper funcs for vertices
bool areVec3Equal(const glm::vec3& a, const glm::vec3& b, float epsilon) {

    return std::abs(a.x - b.x) < epsilon &&
        std::abs(a.y - b.y) < epsilon &&
        std::abs(a.z - b.z) < epsilon;
}


