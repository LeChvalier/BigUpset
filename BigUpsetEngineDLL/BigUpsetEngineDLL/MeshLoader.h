#pragma once

#include <string>

#include "Objects.h"
#include "Renderer.h"

// Forward declarations
struct aiNode;
struct aiScene;
struct aiMesh;

class Renderer;


// Load meshes and parts local matrix
class ModelLoader
{
public:
	ModelLoader() = default;
	ModelLoader(Renderer* renderer) { p_renderer = renderer;};
	~ModelLoader() {};

	// Load mesh and local matrix with Assimp
	// Remember only mesh is loaded, no lights / cameras / textures / animations / ...
	// Return Object to return local matrix
	Mesh LoadMesh(const std::string& file);

private:
	// Need to add meshes
	Renderer* p_renderer;

	// Meshes loading helpers
	//void ProcessNode(Object& object, const aiScene& asScene, const aiNode& asNode);
	void ProcessMesh(Mesh& mesh, const aiScene& asScene, const aiMesh& asMesh);
};