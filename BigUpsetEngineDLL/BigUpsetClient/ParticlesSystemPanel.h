#pragma once

class Engine;
class Game;

struct ParticlesSystemDefinition;
enum class Selected;


class ParticlesSystemPanel
{
public:
	ParticlesSystemPanel() {};
	~ParticlesSystemPanel() {};

	//Render ImGui Particles Panel
	void ImguiRender(Engine* engine, Game* game, Selected& selected);
	void ImguiRenderComponents(Engine* engine, Game* game, Selected& selected);

	float thumbnailSize = 200;

private:
	int m_particleSelected = -1;
	char m_bufName[32];
	char m_bufTexturePath[256];
};