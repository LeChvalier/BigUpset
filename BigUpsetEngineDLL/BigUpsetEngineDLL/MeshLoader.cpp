#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "MeshLoader.h"
#include "Logger.h"

//TODO move to math
mat4 AiToMat4_inverted(aiMatrix4x4 mat)
{
	mat4 toReturn = {
	mat.a1, mat.b1, mat.c1, mat.d1,
	mat.a2, mat.b2, mat.c2, mat.d2,
	mat.a3, mat.b3, mat.c3, mat.d3,
	mat.a4, mat.b4, mat.c4, mat.d4,
	};

	return toReturn;
}

void ModelLoader::ProcessMesh(Mesh& mesh, const aiScene& asScene, const aiMesh& asMesh)
{
	// For each vertex
	for (unsigned int i = 0; i < asMesh.mNumVertices; i++)
	{
		Vertex vertex;
		
		// Fill up positions
		vertex.position = { asMesh.mVertices[i].x, asMesh.mVertices[i].y, asMesh.mVertices[i].z };
		
		// Fill up normals
		if (asMesh.HasNormals())
			vertex.normal = { asMesh.mNormals[i].x, asMesh.mNormals[i].y, asMesh.mNormals[i].z };
		
		// Fill up uv
		if (asMesh.mTextureCoords[0]) // does the mesh contain texture coordinates?
			vertex.uv = { asMesh.mTextureCoords[0][i].x, asMesh.mTextureCoords[0][i].y };
		else
			vertex.uv = { 0.0f, 0.0f };

		// Add current vertex
		mesh.vertices.push_back(vertex);
	}

	// Fill up indices array
	for (unsigned int i = 0; i < asMesh.mNumFaces; i++)
	{
		aiFace face = asMesh.mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			mesh.indices.push_back(face.mIndices[j]);
	}
}

Mesh ModelLoader::LoadMesh(const std::string& file)
{
	Mesh result = {};
	// Read file (Assimp) and check error
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(file.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Error(import.GetErrorString());
		return result;
	}

	//ProcessNode(result, *scene, *scene->mRootNode);
	if (scene->mNumMeshes > 1)
	{
		Logger::Warn("Only first mesh is loaded");
	}
	else if (scene->mNumMeshes < 1)
	{
		Logger::Error("Root node does not contains mesh");
		return result;
	}

	ProcessMesh(result, *scene, *scene->mMeshes[0]);
	result.gpu = p_renderer->CreateMesh(result);
	result.path = file;
	Logger::Info("Load Mesh : " + file);
	return result;
}