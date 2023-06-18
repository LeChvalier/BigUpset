#pragma once

#include <vector>
#include <string>

#include "Maths.h"

// Base class to override for each renderer
struct GPUMesh
{
    unsigned int VAO;
    unsigned int VBO;
    int verticesNumber;
};

struct GPUTexture
{
    int width = 0;
    int height = 0;
    unsigned int texture = 0;
};

// 3D vertices
struct Vertex
{
    // 3D position
    float3 position;
    // Normal vector
    float3 normal;
    // Texture uv
    float2 uv;
};

struct Texture
{
    std::string path = "";

    GPUTexture* gpu = nullptr;
};

struct Mesh
{
    std::string path = "";
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GPUMesh* gpu = nullptr;
};

struct Model
{
    Mesh* mesh;
    Texture* texture;
};
