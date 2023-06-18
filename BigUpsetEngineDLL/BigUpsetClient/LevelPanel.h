#pragma once

#include <string>

class Game;
class Engine;
enum class Selected;

class LevelPanel
{
public:
	void Init(Game* game);

	//Render ImGui Map Panel
	void ImguiRender(Engine* engine, Game* game, Selected& selected);

	float thumbnailSize = 200.f;

private:
	std::vector<std::string> m_listOfEnemyInWave;
	char m_bufAudioPath[256];
	char m_textureMapPath[256];
	char m_gameplayMapPath[256];
	char m_objectMapPath[256];
	char m_bufNameEconomyName[64];
	char m_gameName[64];

	std::string m_selectedDecor_R = "";
	std::string m_selectedDecor_G = "";
	std::string m_selectedDecor_B = "";

};
