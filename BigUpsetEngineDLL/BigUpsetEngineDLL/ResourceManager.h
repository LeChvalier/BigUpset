#pragma once

#include <memory>
#include <string>
#include <list>

#include "TextureLoader.h"
#include "MeshLoader.h"
#include "Objects.h"

// Assimp forward declarations
struct aiNode;
struct aiScene;
struct aiMesh;
enum class GroundType;

// Stock resources and give correspondant Ids to the Scene
class ResourceManager
{
public:
	ResourceManager(Renderer* renderer);
	~ResourceManager();

	// Load assets
	Mesh* AddMesh(const char* meshFilename);
	Texture* AddTexture(const char* textureFilename);

	// Parse the map noise to get rgb/rgba values
	std::unique_ptr<float4[]> ParseMap(const std::string& texture, unsigned int& w, unsigned int& h) { return textureLoader.ParseMap(texture, w, h); };

private:
	// Loaders
	TextureLoader textureLoader;
	ModelLoader modelLoader;

	// Loaded Resources
	// Using std::list for stable pointers
	std::list<Texture> textures;
	std::list<Mesh> meshes;
};