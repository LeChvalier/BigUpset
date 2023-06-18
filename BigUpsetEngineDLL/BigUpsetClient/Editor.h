#pragma once

//imgui Include
#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BigUpsetEngine.h"
#include "GameDLL.h"

#include "ContentBrowserPanel.h"
#include "EntityPanel.h"
#include "ScenePanel.h"
#include "GamePanel.h"
#include "LoggerPanel.h"
#include "LevelPanel.h"
#include "ParticlesSystemPanel.h"


enum class Selected
{
	E_Character,
	E_Building,
	E_Particle,
	E_Map,
	E_Decor,
	E_None
};

class Editor
{
public:
	Editor(Engine* _engine, Game* _game, GLFWwindow* _window);
	~Editor();

	//Init Editor
	void Init();

	Engine* engine = nullptr;
	Game* game = nullptr;

	ImGuiDockNodeFlags dockspace_flags;

	bool dockSpaceOpen = true;
	bool opt_fullscreen = true;
	bool opt_padding = false;

	//Update Editor
	void Update(const float2& windowSize);
	//Update ImGui Editor
	void UpdateImgui(const float2& windowSize);
	//Update MenuBar
	void WidgetMenuBar();
	//Update Scene Screen in editor
	void WidgetScene();
	//Update Game Screen in editor
	void WidgetGame();
	//Update ContentBrowser
	void WidgetResources();
	//Update Entities manager
	void WidgetEntities();
	//Update ParticleSystem manager
	void WidgetParticlesSystem();
	//VOID, The components system update is in the WidgetEntities !, this fuction just init the window actually
	void WidgetComponents();
	//Update console editor
	void WidgetConsole();
	//Update Level Settings
	void WidgetLevel();

	float2 gameScreenSize = { 0.f, 0.f };
	float2 gameScreenPos {0.f, 0.f};

private:
	//Set BigUpset Theme
	void Style();

	Selected selected = Selected::E_None;

	ContentBrowserPanel m_ContentBrowserPanel;
	EntityPanel m_EntityPanel;
	ScenePanel m_ScenePanel;
	GamePanel m_GamePanel;
	LoggerPanel m_LoggerPanel;
	LevelPanel m_LevelPanel;
	ParticlesSystemPanel m_ParticlePanel;

	// FOR LOG CONSOLE
};


