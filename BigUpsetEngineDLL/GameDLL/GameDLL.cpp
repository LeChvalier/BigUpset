#include <imgui.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <iostream>

#include "TemplateManager.h"
#include "BigUpsetEngine.h"
#include "GameDLL.h"
#include "Entity.h"
#include "Debug.h"


Game::Game(Engine* _engine, GLFWwindow* window)
{
	//assert(false);

	mousePicking = MousePicking(&_engine->renderer, &engine->resourceManager);
	miniMap = MiniMap(&_engine->renderer, &engine->resourceManager);
	if (window == nullptr)
	{
		Logger::Critical("Error with window creation in game");
	}

	engine = _engine;

	////Setup Dear ImGui context
	//IMGUI_CHECKVERSION();
	//
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//ImGui::StyleColorsDark();
	//ImGuiStyle& style = ImGui::GetStyle();
	//if (io.ConfigFlags)
	//{
	//	style.WindowRounding = 0.0f;
	//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	//}

	//ImGui_ImplGlfw_InitForOpenGL(window, true);
	//ImGui_ImplOpenGL3_Init("#version 450");

	for (auto& directoryEntry : std::filesystem::directory_iterator(PARTICLE))
	{
		std::string name = directoryEntry.path().stem().string();
		ParticlesSystemDefinition particle = TemplateManager::LoadParticle(name);

		particle.m_id = m_numberIdParticles;

		m_particlesTemplate.push_back(particle);
		m_numberIdParticles += 1;
	}

	for (auto& directoryEntry : std::filesystem::directory_iterator(CHARACTER))
	{
		std::string name = directoryEntry.path().stem().string();

		// TODO: Make the UnitDefinition creation in one line
		UnitDefinition newChara = TemplateManager::LoadCharacter(name);
		newChara.model.mesh = _engine->resourceManager.AddMesh(newChara.pathMesh.c_str());
		newChara.model.texture = _engine->resourceManager.AddTexture(newChara.pathTexture.c_str());

		for (ParticlesSystemDefinition particle : m_particlesTemplate)
		{
			if (particle.m_id == newChara.m_IdAttackParticle)
				newChara.m_attackParticle = particle;
			if (particle.m_id == newChara.m_IdDieParticle)
				newChara.m_dieParticle = particle;
		}

		newChara.m_id = m_numberIdCharacter;

		m_charactersTemplate.push_back(newChara);
		m_numberIdCharacter += 1;
	}

	for (auto& directoryEntry : std::filesystem::directory_iterator(BUILDING))
	{
		std::string name = directoryEntry.path().stem().string();

		// TODO: Make the UnitDefinition creation in one line
		UnitDefinition newbuild = TemplateManager::LoadBuilding(name);
		newbuild.model.mesh = _engine->resourceManager.AddMesh(newbuild.pathMesh.c_str());
		newbuild.model.texture = _engine->resourceManager.AddTexture(newbuild.pathTexture.c_str());

		for (ParticlesSystemDefinition particle : m_particlesTemplate)
		{
			if (particle.m_id == newbuild.m_IdAttackParticle)
				newbuild.m_attackParticle = particle;
			if (particle.m_id == newbuild.m_IdDieParticle)
				newbuild.m_dieParticle = particle;
		}

		newbuild.m_id = m_numberIdBuildings;

		m_buildingsTemplate.push_back(newbuild);
		m_numberIdBuildings += 1;
	}

	//pour les decorEntities :
	for (auto& directoryEntry : std::filesystem::directory_iterator(DECOR))
	{
		std::string name = directoryEntry.path().stem().string();
		DecorDefinition decor = TemplateManager::LoadDecor(name);
		decor.model.mesh = _engine->resourceManager.AddMesh(decor.pathMesh.c_str());
		decor.model.texture = _engine->resourceManager.AddTexture(decor.pathTexture.c_str());

		decor.m_id = m_numberIdDecors;

		m_decorsTemplate.push_back(decor);
		m_numberIdDecors += 1;
	}


	engine->scene.camera = TemplateManager::LoadCamera("Camera");

	soundInfo = TemplateManager::LoadSoundInfo("soundInfo");
	waveInfo = TemplateManager::LoadWaveInfo("waveInfo");
	economySystem = TemplateManager::LoadEconomySystem("economy");
	engine->scene.map = TemplateManager::LoadMap("map");
	name = TemplateManager::LoadGeneral("general");

}

void Game::Init(GLFWwindow* window, float2 windowSize)
{
	mousePicking.Init(window, windowSize);
	miniMap.Init(window, windowSize);
	economySystem.SetMoney(economySystem.startingMoney);
}

void Game::newCharacterTemplate()
{
	UnitDefinition c = {};
	c.m_id = m_numberIdCharacter;
	c.m_type = UT_CHARACTER;
	c.model.mesh = engine->resourceManager.AddMesh(c.pathMesh.c_str());
	c.model.texture = engine->resourceManager.AddTexture(c.pathTexture.c_str());
	c.Save();
	m_charactersTemplate.push_back(c);
	m_numberIdCharacter += 1;
}

void Game::newBuildingsTemplate()
{
	UnitDefinition b = {};
	b.m_id = m_numberIdBuildings;
	b.m_type = UT_BUILDING;
	b.model.mesh = engine->resourceManager.AddMesh(b.pathMesh.c_str());
	b.model.texture = engine->resourceManager.AddTexture(b.pathTexture.c_str());
	b.Save();
	m_buildingsTemplate.push_back(b);
	m_numberIdBuildings += 1;
}

void Game::newDecorTemplate()
{
	DecorDefinition d = {};
	d.m_id = m_numberIdDecors;
	d.model.mesh = engine->resourceManager.AddMesh(d.pathMesh.c_str());
	d.model.texture = engine->resourceManager.AddTexture(d.pathTexture.c_str());
	d.Save();
	m_decorsTemplate.push_back(d);
	m_numberIdDecors += 1;
}

void Game::newParticleTemplate()
{
	ParticlesSystemDefinition p = {};
	p.m_id = m_numberIdParticles;
	p.Save();
	std::cout << "TODO : Add Save fct for save and load particleTemplate" << std::endl;
	m_particlesTemplate.push_back(p);
	m_numberIdParticles += 1;
}

void Game::uiUpdate(const float2& windowSize)
{
	miniMapUpdate(windowSize);
	overlayEntityUpdate();
	overlayGlobalInfoUpdate();
}

void Game::overlayGlobalInfoUpdate()
{
	const float DISTANCE = 10.0f;
	static int corner = 0;

	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	if (corner != -1)
		window_flags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("OverlayGlobalInfo", &m_overlayOpen, window_flags))
	{
		ImGui::Text(economySystem.name.c_str());
		ImGui::SameLine();
		ImGui::Text(":");
		ImGui::SameLine();
		ImGui::Text(std::to_string(economySystem.GetMoney()).c_str());

		if (wavesystem.gameFinish)
		{
			ImGui::Text("ALL ENEMIES ARE DESTROYED !");
		}
		else if (wavesystem.currentEnemyNumber > 0)
		{
			ImGui::Text("ENEMY IN COMING ! Number of Enemies : ");
			ImGui::SameLine();
			ImGui::Text(std::to_string(wavesystem.currentEnemyNumber).c_str());
		}
		else if (wavesystem.waveRound < waveInfo.maxWave)
		{
			ImGui::Text("NEXT WAVE IN : ");
			ImGui::SameLine();
			ImGui::Text(std::to_string(waveInfo.wave[wavesystem.waveRound].timerBeforeWave - wavesystem.m_timer.GetDuration()).c_str());
			ImGui::SameLine();
			ImGui::Text(" seconds");
		}

		ImGui::Text("WAVE : ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(wavesystem.waveRound).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(waveInfo.maxWave).c_str());
	}

	ImGui::End();
}

void Game::miniMapUpdate(const float2& windowSize)
{
	const float DISTANCE = 10.0f;
	static int corner = 3;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	//ImGui::setnext
	ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration/* | ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1)
		window_flags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("OverlayMiniMap", &m_overlayOpen, window_flags))
	{
		ImGui::SetWindowSize(ImVec2(windowSize.x / 4.f, windowSize.y / 4.f));
		if (!DEBUGMODE)
			ImGui::Image((ImTextureID)(size_t)miniMap.GetCBO(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		else
			ImGui::Image((ImTextureID)(size_t)engine->renderer.pickingCBO, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}

void Game::overlayEntityUpdate()
{
	const float DISTANCE = 10.0f;
	static int corner = 2;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1)
		window_flags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Overlay", &m_overlayOpen, window_flags) && gameInfo.selectedEntity != -1)
	{
		for (UnitEntity& unit : engine->scene.characterEntities)
		{
			if (unit.gameID == gameInfo.selectedEntity)
			{
				ImGui::Text(unit.definition.m_name.c_str());
				ImGui::Separator();
				std::string temp = "Life: ";
				temp += std::to_string(unit.definition.m_life);
				ImGui::Text(temp.c_str());
				temp = "Damage: ";
				temp += std::to_string(unit.definition.m_damage);
				ImGui::Text(temp.c_str());
			}
		}
		for (UnitEntity& unit : engine->scene.buildingEntities)
		{
			if (unit.gameID == gameInfo.selectedEntity)
			{
				ImGui::Text(unit.definition.m_name.c_str());
				ImGui::Separator();
				std::string temp = "Life: ";
				temp += std::to_string(unit.definition.m_life);
				ImGui::Text(temp.c_str());
				temp = "Damage: ";
				temp += std::to_string(unit.definition.m_damage);
				ImGui::Text(temp.c_str());


				if (unit.definition.m_mainBuilding)
				{
					ImGui::Separator();
					for (UnitDefinition& build : m_buildingsTemplate)
					{
						if (!build.m_mainBuilding)
						{
							if (build.m_side == AISideType::ALLIED)
							{
								if (ImGui::Button(build.m_name.c_str()))
								{
									if (economySystem.GetMoney() > build.m_price)
									{
										gameInfo.selectedCreatableBuilding = build.m_id;
										economySystem.RemoveMoney(build.m_price);
									}
								}
								std::string cost = std::to_string(build.m_price) + " " + economySystem.name;
								ImGui::SameLine();
								ImGui::Text(cost.c_str());
							}
						}
					}
				}

				ImGui::Separator();

				if (unit.definition.m_canCreateCharacter)
				{
					UnitDefinition* chara = nullptr;
					for (UnitDefinition& unitTemp : m_charactersTemplate)
					{
						if (unitTemp.m_id == unit.definition.m_IdUnitDefCreateable)
						{
							chara = &unitTemp;
						}
					}

					if (ImGui::Button("Create Unit"))
					{
						//for (UnitDefinition& chara : m_charactersTemplate)

						if (economySystem.GetMoney() > chara->m_price)
						{
							float3 newPos(unit.position.x + 0.8f, 0, unit.position.z + 0.8f);
							engine->scene.NewUnit(*chara, newPos);
							economySystem.RemoveMoney(chara->m_price);
						}
					}
					std::string cost = std::to_string(chara->m_price) + " " + economySystem.name;
					ImGui::SameLine();
					ImGui::Text(cost.c_str());
				}
			}
		}

		ImGui::Image((ImTextureID)(size_t)engine->preview.GetCBO(), ImVec2(100.f, 100.f), ImVec2(0, 1), ImVec2(1, 0));


		/*if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			ImGui::EndPopup();
		}*/
	}
	ImGui::End();
}

void Game::CreateGame()
{
	// Initialize Map

	// Always a quad
	engine->scene.map.model.mesh = engine->resourceManager.AddMesh(QUAD);

	engine->scene.map.model.texture = engine->resourceManager.AddTexture(engine->scene.map.model.texture->path.c_str());
	engine->scene.map.tiles = engine->resourceManager.ParseMap(engine->scene.map.gameplayMap, engine->scene.map.tilesWidth, engine->scene.map.tilesHeight);

	engine->scene.map.initialized = true;

	engine->scene.map.SpawnDecor(&engine->resourceManager, m_decorsTemplate, &engine->scene);

	std::srand(std::time(nullptr));
	for (UnitDefinition& build : m_buildingsTemplate)
	{
		if (build.m_mainBuilding)
		{
			//float x = std::rand() % 20;
			//float z = std::rand() % 20;
			engine->scene.NewUnit(build, { 10 , 0, 10 });
		}
	}

	// TODO delete the following line
	//engine->scene.NewUnit(m_charactersTemplate.front(), { 0.f , 0.f, 0.f });

	engine->scene.soundManager.PlayMusic(soundInfo.ambientMusic, soundInfo.volume_AmbientMusic);

	engine->preview.hide = true;

}

void Game::gameUpdate(GLFWwindow* window)
{
	camMovement(window);

	economySystem.Update();
	wavesystem.WavesUpdate(&engine->scene, waveInfo, m_charactersTemplate);

	//if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	//{
	//	DEBUGMODE = !DEBUGMODE;

	//	/*if (DEBUGMODE)
	//		Logger::Info("Debug enable");
	//	else
	//		Logger::Info("Debug disable");*/
	//}
}

void Game::camMovement(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			engine->scene.camera.position.x -= engine->scene.camera.fastSpeed * deltaTime;
		else
			engine->scene.camera.position.x -= engine->scene.camera.normalSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			engine->scene.camera.position.z -= engine->scene.camera.fastSpeed * deltaTime;
		else
			engine->scene.camera.position.z -= engine->scene.camera.normalSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			engine->scene.camera.position.x += engine->scene.camera.fastSpeed * deltaTime;
		else
			engine->scene.camera.position.x += engine->scene.camera.normalSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			engine->scene.camera.position.z += engine->scene.camera.fastSpeed * deltaTime;
		else
			engine->scene.camera.position.z += engine->scene.camera.normalSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		engine->scene.camera.position.y -= engine->scene.camera.normalSpeed * deltaTime;

	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		engine->scene.camera.position.y += engine->scene.camera.normalSpeed * deltaTime;
	}

	// Disable out of bound
	engine->scene.camera.position.y = Math::Clamp(engine->scene.camera.position.y, MIN_HEIGHT, MAX_HEIGHT);
	engine->scene.camera.position.x = Math::Clamp(engine->scene.camera.position.x, -engine->scene.map.scale.x / 2.f, engine->scene.map.scale.x / 2.f);
	engine->scene.camera.position.z = Math::Clamp(engine->scene.camera.position.z, -engine->scene.map.scale.z / 2.f, engine->scene.map.scale.z / 2.f);
}


void Game::ResetGame()
{
	engine->scene.CleanScene();
}
