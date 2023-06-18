#include <stdlib.h>
#include <imgui.h>

#include "BigUpsetEngine.h"
#include "GameDLL.h"

#include "SoundManager.h"
#include "GamePanel.h"
#include "Logger.h"
#include "Debug.h"


GamePanel::GamePanel()
{

}

GamePanel::~GamePanel()
{

}

void GamePanel::ImguiRender(Game* game, float2& gameWindowSize, float2& gameScreenPos)
{
	if (ImGui::Begin("Game"))
	{
		ImVec2 ImWinSize = ImGui::GetContentRegionAvail();
		gameWindowSize.x = ImWinSize.x;
		gameWindowSize.y = ImWinSize.y;

		ImVec2 ImWinPos = ImGui::GetWindowPos();
		gameScreenPos.x = ImWinPos.x;
		gameScreenPos.y = ImWinPos.y;


		if (ImGui::Button("Play"))
		{
			if (game->InGame)
			{
				unsigned int cbo = game->engine->scene.GetCBO();
				ImGui::Image((void*)(intptr_t)(cbo), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::End();
				return;
			}

			game->CreateGame();

			game->InGame = true;

			//game->engine->scene.soundManager.PlayMusic(game->soundInfo.ambientMusic, game->soundInfo.volume_AmbientMusic);
			game->wavesystem.ResetWave();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			game->ResetGame();
			game->engine->scene.camera = TemplateManager::LoadCamera("Camera");
			game->engine->scene.soundManager.StopMusic();
			game->InGame = false;
		}

		// Draw game
		ImGui::Image((void*)(intptr_t)(game->engine->scene.GetCBO()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}
