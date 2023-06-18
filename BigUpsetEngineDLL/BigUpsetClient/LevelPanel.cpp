#include <imgui.h>

#include "BigUpsetEngine.h"
#include "GameDLL.h"

#include "TemplateManager.h"
#include "levelPanel.h"
#include "Editor.h"
#include "Map.h"

void LevelPanel::Init(Game* game)
{
	// Fill up decorTemplateName
	for (auto& decorTemplate : game->m_decorsTemplate)
	{
		if (game->engine->scene.map.decor_R_id == decorTemplate.m_id)
			m_selectedDecor_R = decorTemplate.m_name;

		if (game->engine->scene.map.decor_G_id == decorTemplate.m_id)
			m_selectedDecor_G = decorTemplate.m_name;

		if (game->engine->scene.map.decor_B_id == decorTemplate.m_id)
			m_selectedDecor_B = decorTemplate.m_name;
	}
};

void LevelPanel::ImguiRender(Engine* engine, Game* game, Selected& selected)
{
	if (ImGui::Begin("General Settings"))
	{
		ImGui::Text("GAME TITLE");

		strncpy(m_gameName, game->name.c_str(), sizeof(m_gameName) - 1);
		ImGui::InputText("Game Title", m_gameName, IM_ARRAYSIZE(m_gameName));
		game->name = m_gameName;

		if (ImGui::Button("Save Game Name"))
		{
			TemplateManager::WriteGeneral(game->name, "general");
		}

		if (ImGui::Button("ActualMap", { thumbnailSize, thumbnailSize }))
		{
			selected = Selected::E_Map;
		}

		ImGui::Separator();

		ImGui::Text("AMBIENT MUSIC");

		strncpy(m_bufAudioPath, game->soundInfo.ambientMusic.c_str(), sizeof(m_bufAudioPath) - 1);
		ImGui::InputText("File Path", m_bufAudioPath, IM_ARRAYSIZE(m_bufAudioPath));
		game->soundInfo.ambientMusic = m_bufAudioPath;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_SOUND"))
			{
				game->soundInfo.ambientMusic = std::string((const char*)payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Text("Warning: In order for your music to work \ncorrectly, please make sure that you write \nthe path of the file correctly. Do not put \nthe 'assets/'. Use ONLY .OGG FILE");

		ImGui::SliderFloat("Volume", &game->soundInfo.volume_AmbientMusic, 0.f, 2.f);

		if (ImGui::Button("Save audio settings"))
		{
			TemplateManager::WriteSoundInfo(game->soundInfo, "soundInfo");
		}

		ImGui::Separator();

		ImGui::Text("WAVES SETTINGS");

		ImGui::InputFloat("Spawn Point X", &game->waveInfo.EnemySpawnPointX);
		ImGui::InputFloat("Spawn Point Z", &game->waveInfo.EnemySpawnPointZ);
		ImGui::InputFloat("Spawn Range", &game->waveInfo.EnemySpawnRange);

		ImGui::InputInt("Number of Wave", &game->waveInfo.maxWave);

		if (game->waveInfo.wave.size() < game->waveInfo.maxWave)
		{
			Wave wave;
			game->waveInfo.wave.push_back(wave);
		}
		else if (game->waveInfo.wave.size() > game->waveInfo.maxWave)
		{
			game->waveInfo.wave.pop_back();
		}

		if (game->waveInfo.maxWave >= 1)
			for (int i = 0; i != game->waveInfo.wave.size(); i++)
			{
				ImGui::Separator();
				std::string strName = "WAVE : " + std::to_string(i + 1);
				ImGui::Text(strName.c_str());

				std::string timertxt = "WAVE " + std::to_string(i + 1) + ": Time Before Wave";
				ImGui::InputFloat(timertxt.c_str(), &game->waveInfo.wave[i].timerBeforeWave);

				std::string numberEnemiestxt = "WAVE " + std::to_string(i + 1) + ": Number of Enemy";
				ImGui::InputInt(numberEnemiestxt.c_str(), &game->waveInfo.wave[i].EnemiesNumber);

				if (game->waveInfo.wave[i].enemies.size() < game->waveInfo.wave[i].EnemiesNumber)
				{
					EnemiesWave enemiWave;
					game->waveInfo.wave[i].enemies.push_back(enemiWave);
				}
				else if (game->waveInfo.wave[i].enemies.size() > game->waveInfo.wave[i].EnemiesNumber)
				{
					game->waveInfo.wave[i].enemies.pop_back();
				}

				if (game->waveInfo.wave[i].enemies.size() > 0)
					for (int j = 0; j < game->waveInfo.wave[i].EnemiesNumber; j++)
					{
						std::string enemyNametxt = "WAVE " + std::to_string(i + 1) + ": Enemy " + std::to_string(j + 1) + " Name";

						static std::string actualunit = "";
						for (UnitDefinition& unit : game->m_charactersTemplate)
						{
							if (unit.m_name == game->waveInfo.wave[i].enemies[j].name)
							{
								actualunit = unit.m_name;
							}
						}

						if (ImGui::BeginCombo(enemyNametxt.c_str(), actualunit.c_str()))
						{
							for (UnitDefinition& unit : game->m_charactersTemplate)
							{
								if (unit.m_side == AISideType::ENEMY && ImGui::Selectable(unit.m_name.c_str()))
								{
									actualunit = unit.m_name;
									game->waveInfo.wave[i].enemies[j].name = unit.m_name;
								}
							}

							ImGui::EndCombo();
						}

						std::string enemyNumbertxt = "WAVE " + std::to_string(i + 1) + ": Enemy " + std::to_string(j + 1) + " Number";
						ImGui::InputInt(enemyNumbertxt.c_str(), &game->waveInfo.wave[i].enemies[j].number);
						
						if (game->waveInfo.wave[i].enemies[j].number < 1)
							game->waveInfo.wave[i].enemies[j].number = 1;
					}
				else if(game->waveInfo.wave[i].EnemiesNumber < 1)
				{
					game->waveInfo.wave[i].EnemiesNumber = 1;
				}
			}
		else if (game->waveInfo.maxWave < 1)
		{
			game->waveInfo.maxWave = 1;
		}

		if (ImGui::Button("Save Parameters"))
		{
			TemplateManager::WriteWaveInfo(game->waveInfo, "waveInfo");
		}
	}
	ImGui::End();

	ImGui::Begin("Components");
	if (selected == Selected::E_Map)
	{
		if (!game->InGame)
		{
			ImGui::SliderFloat3("Cam Pos", &game->engine->scene.camera.position.x, -50.f, 50.f);
			ImGui::SliderFloat("Cam Pitch", &game->engine->scene.camera.pitch, -Math::PI * 2.f, 2.f * Math::PI);
			ImGui::SliderFloat("Cam Yaw", &game->engine->scene.camera.yaw, -Math::PI * 2.f, 2.f * Math::PI);
			ImGui::SliderFloat("Cam Roll", &game->engine->scene.camera.roll, -Math::PI * 2.f, 2.f * Math::PI);

			ImGui::Separator();

			ImGui::SliderFloat("Normal Speed", &game->engine->scene.camera.normalSpeed, 1.f, 10.f);
			ImGui::SliderFloat("Fast Speed", &game->engine->scene.camera.fastSpeed, 1.f, 15.f);

			ImGui::Separator();

			if (ImGui::Button("Save Camera"))
			{
				TemplateManager::WriteCamera(game->engine->scene.camera, "Camera");
			}

			ImGui::Separator();

			//ImGui::SliderFloat3("Map Position", &game->engine->scene.map.position.x, -50.f, 50.f);
			//ImGui::SliderFloat3("Map Rotation", &game->engine->scene.map.rotation.x, -Math::PI / 2.f, Math::PI / 2.f);
			ImGui::SliderFloat("Map Scale X", &game->engine->scene.map.scale.x, 1.f, 100.f);
			ImGui::SliderFloat("Map Scale Z", &game->engine->scene.map.scale.z, 1.f, 100.f);

			ImGui::SliderFloat("SpawnValue", &game->engine->scene.map.spawnValue, 0.f, 100.f);
			ImGui::SliderFloat3("Floor Value", &game->engine->scene.map.floorValue.x, 0.f, 255.f);

			ImGui::Text("Decors_R :");
			if (ImGui::BeginCombo("Decors_R", m_selectedDecor_R.c_str()))
			{
				for (DecorDefinition& decor : game->m_decorsTemplate)
					if (ImGui::Selectable(decor.m_name.c_str()))
					{
						m_selectedDecor_R = decor.m_name;
						game->engine->scene.map.decor_R_id = decor.m_id;

						Logger::Warn("Don't forget to save entity");
					}
				ImGui::EndCombo();
			}

			ImGui::Text("Decors_G :");
			if (ImGui::BeginCombo("Decors_G", m_selectedDecor_G.c_str()))
			{
				for (DecorDefinition& decor : game->m_decorsTemplate)
					if (ImGui::Selectable(decor.m_name.c_str()))
					{
						m_selectedDecor_G = decor.m_name;

						game->engine->scene.map.decor_G_id = decor.m_id;

						Logger::Warn("Don't forget to save entity");

					}
				ImGui::EndCombo();
			}

			ImGui::Text("Decors_B :");
			if (ImGui::BeginCombo("Decors_B", m_selectedDecor_B.c_str()))
			{
				for (DecorDefinition& decor : game->m_decorsTemplate)
					if (ImGui::Selectable(decor.m_name.c_str()))
					{
						m_selectedDecor_B = decor.m_name;
						game->engine->scene.map.decor_B_id = decor.m_id;

						Logger::Warn("Don't forget to save entity");
					}
				ImGui::EndCombo();
			}

			strncpy(m_textureMapPath, game->engine->scene.map.model.texture->path.c_str(), sizeof(m_textureMapPath) - 1);
			ImGui::InputText("Texture map", m_textureMapPath, IM_ARRAYSIZE(m_textureMapPath));
			game->engine->scene.map.model.texture->path = m_textureMapPath;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
				{
					game->engine->scene.map.model.texture->path = ASSETS + std::string((const char*)payload->Data);
					game->engine->scene.map.model.texture = game->engine->resourceManager.AddTexture(game->engine->scene.map.model.texture->path.c_str());
					Logger::Warn("Don't forget to save entity");
				}
				ImGui::EndDragDropTarget();
			}

			strncpy(m_gameplayMapPath, game->engine->scene.map.gameplayMap.c_str(), sizeof(m_gameplayMapPath) - 1);
			ImGui::InputText("Gameplay map", m_gameplayMapPath, IM_ARRAYSIZE(m_gameplayMapPath));
			game->engine->scene.map.gameplayMap = m_gameplayMapPath;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
				{
					game->engine->scene.map.gameplayMap = ASSETS + std::string((const char*)payload->Data);

					game->engine->scene.map.tiles = game->engine->resourceManager.ParseMap(game->engine->scene.map.gameplayMap, game->engine->scene.map.tilesWidth, game->engine->scene.map.tilesHeight);
					Logger::Warn("Don't forget to save entity");

				}
				ImGui::EndDragDropTarget();
			}

			strncpy(m_objectMapPath, game->engine->scene.map.objectMap.c_str(), sizeof(m_objectMapPath) - 1);
			ImGui::InputText("Object map", m_objectMapPath, IM_ARRAYSIZE(m_objectMapPath));
			game->engine->scene.map.objectMap = m_objectMapPath;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
				{
					game->engine->scene.map.objectMap = ASSETS + std::string((const char*)payload->Data);

					Logger::Warn("Don't forget to save entity");
				}
				ImGui::EndDragDropTarget();
			}


			ImGui::SliderInt("Seed", &game->engine->scene.map.seed, 0, 100);

			if (ImGui::Button("Save map settings"))
			{
				TemplateManager::WriteMap(game->engine->scene.map, "map");
			}

			ImGui::Separator();
			ImGui::Text("Economy");

			ImGui::SliderInt("Max money", &game->economySystem.maxMoney, 0, 10000);
			ImGui::SliderInt("Money per second", &game->economySystem.moneyPerSec, 0, 1000);
			ImGui::SliderInt("Starting money", &game->economySystem.startingMoney, 0, 10000);

			strncpy(m_bufNameEconomyName, game->economySystem.name.c_str(), sizeof(m_bufNameEconomyName) - 1);
			ImGui::InputText("Economy name", m_bufNameEconomyName, IM_ARRAYSIZE(m_bufNameEconomyName));
			game->economySystem.name = m_bufNameEconomyName;

			if (ImGui::Button("Save economy settings"))
			{
				TemplateManager::WriteEconomySystem(game->economySystem, "economy");
			}
		}
	}
	ImGui::End();
}