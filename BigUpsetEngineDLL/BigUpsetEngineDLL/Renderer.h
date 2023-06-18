#pragma once

#include "Maths.h"

#include "ShaderLoader.h"
#include "Objects.h"
#include "Camera.h"


// Forward declarations
class Camera;
class Object_indice;

struct Mesh;
struct Texture;
struct GPUMesh;
struct GPUTexture;
struct Model;


class Renderer
{
public:
	Renderer();
	~Renderer();

	// Compile mesh
	static GPUMesh* CreateMesh(const Mesh& mesh);
	// Compile texture
	static GPUTexture* CreateTexture(const Texture& texture);

	// Draw Scene
	void DrawObject(const mat4& model, const Model& object);
	void EndDraw();

	// The CBO is used by the editor to display preview/game
	// FrameBuffer pointer
	unsigned int FBO = 0;
	// Color buffer
	unsigned int CBO = 0;
	// Depth stencil buffer
	unsigned int RBO = 0;
	
	void StartDraw(const mat4& proj, const mat4& view, int _fbo, float2 size, float4 cleanColor = {0.f, 0.f, 0.f, 1.f});
	
	// Render a quad to fill up the FBO
	void RenderQuad();

	unsigned int m_gameProgram = 0;

	// To render Preview
	unsigned int previewFBO = 0;
	unsigned int previewCBO = 0;
	unsigned int previewRBO = 0;

	// To render Picking
	unsigned int pickingFBO = 0;
	unsigned int pickingCBO = 0;
	unsigned int pickingRBO = 0;

	// To render MiniMap
	unsigned int miniMapFBO = 0;
	unsigned int miniMapCBO = 0;
	unsigned int miniMapRBO = 0;

	
	void UpdateCBORBO(unsigned int& RBO, unsigned int& CBO, float2 size);

private:
	// Initialize the FBO
	void CreateFBO(unsigned int& FBO, unsigned int& CBO, unsigned int& RBO);
	void UpdateRBOs(float2 size);

	// Shader program
	unsigned int m_program = 0;

	ShaderLoader* shaderLoader;
};