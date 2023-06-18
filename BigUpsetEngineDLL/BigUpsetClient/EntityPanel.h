#pragma once

#include <list>

class Engine;
class Game;

struct DecorDefinition;
struct UnitDefinition;
enum class Selected;

class EntityPanel
{
public:
	EntityPanel();
	~EntityPanel();

	//Render ImGui Entities Panel
	void ImguiRender(Game* game, Engine* engine, Selected& selected);
	//Render ImGui Components Panel
	void ImguiRenderComponents(Game* game, Engine* engine, Selected& selected);

	int characterSelectedId = 0;
	int buildingSelectedId = 0;
	int decorSelectedId = 0;
	char bufName[32];
	char bufMeshPath[256];
	char bufTexturePath[256];
	char bufShotSoundPath[256];
	char bufDieSoundPath[256];
	float thumbnailSize = 200;
	
private:
	void ListBoxUnit(const char* name, std::list<UnitDefinition>& units, int& selectedUnitID, Engine* engine, Selected& selected);
	void ListBoxDecor(const char* name, std::list<DecorDefinition>& decors, int& selectedUnitID, Engine* engine, Selected& selected);

	bool m_sideSelected = false;
	bool m_scriptSelected = false;
};
