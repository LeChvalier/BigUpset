#include <iostream>
#include <vector>

#include "ResourceManager.h"
#include "Logger.h"

ResourceManager::ResourceManager(Renderer* renderer)
	: textureLoader(renderer)
	, modelLoader(renderer)
{
}

Mesh* ResourceManager::AddMesh(const char* meshFilename)
{
	for (Mesh& testMesh : meshes)
	{
		if (testMesh.path == meshFilename)
		{
			return &testMesh;
		}
	}

	Mesh mesh = modelLoader.LoadMesh(meshFilename);
	meshes.push_back(mesh);
	return &meshes.back();
}

Texture* ResourceManager::AddTexture(const char* textureFilename)
{
	for (Texture& testTexture : textures)
	{
		if (testTexture.path == textureFilename)
		{
			return &testTexture;
		}
	}

	Texture texture = textureLoader.LoadTexture(textureFilename);
	textures.push_back(texture);
	return &textures.back();
}

ResourceManager::~ResourceManager()
{
	for (Texture text : textures)
	{
		delete text.gpu;
	}

	for (Mesh mesh : meshes)
	{
		delete mesh.gpu;
	}
}
