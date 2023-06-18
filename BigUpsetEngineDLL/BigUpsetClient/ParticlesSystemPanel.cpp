#include <imgui.h>

#include "BigUpsetEngine.h"
#include "GameDLL.h"

#include "Editor.h"
#include "Maths.h"

#include "ParticlesSystemPanel.h"

#include "Define.h"

void ParticlesSystemPanel::ImguiRender(Engine* engine, Game* game, Selected& selected)
{
	if (selected != Selected::E_Particle)
		m_particleSelected = -1;

	if (ImGui::Begin("Particle System"))
	{
		ImGui::Separator();
		if (ImGui::ListBoxHeader("Particles systeme :", { thumbnailSize, (thumbnailSize / 6) * game->m_particlesTemplate.size() + 20 }))
		{
			int j = 0;
			for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
			{
				ImGui::PushID(j);

				const std::string& item_name = particle.m_name;

				if (ImGui::Selectable(item_name.c_str(), j == m_particleSelected, 0, { thumbnailSize, thumbnailSize / 6 }))
				{
					m_particleSelected = j;

					if (particle.m_id == m_particleSelected)
					{
						ParticlesSystem particleSystem(&game->engine->resourceManager, particle);
						engine->preview.ShowParticles(particleSystem);

						selected = Selected::E_Particle;
					}
				}
				j++;
				ImGui::PopID();

				// TODO drag and drop 

			}
			ImGui::EndListBox();
		}
	}

	ImGui::Separator();

	if (ImGui::Button("New Particle", { thumbnailSize, thumbnailSize / 6 }))
	{
		bool sameName = false;
		for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
		{
			if (particle.m_name == "Default name")
				sameName = true;
		}
		if (sameName)
			Logger::Error("Default name already exist, rename it and save it for create new particle");
		else
		{
			game->newParticleTemplate();

			m_particleSelected = game->m_particlesTemplate.size() - 1;
			//engine->preview.ShowModel(game->m_particlesTemplate.back().model); // TODO put particle
		}
		selected = Selected::E_Particle;
	}

	ImGui::End();
	ImguiRenderComponents(engine, game, selected);
}

void ParticlesSystemPanel::ImguiRenderComponents(Engine* engine, Game* game, Selected& selected)
{
	assert(game != nullptr);

	ImGui::Begin("Components");

	static std::string lastName;

	for (auto it = game->m_particlesTemplate.begin(); it != game->m_particlesTemplate.end();)
	{
		ParticlesSystemDefinition& particle = *it;

		if (particle.m_id == m_particleSelected)
		{
			strncpy(m_bufName, particle.m_name.c_str(), sizeof(m_bufName) - 1);

			ImGui::InputText("name", m_bufName, IM_ARRAYSIZE(m_bufName));
			if (ImGui::IsItemDeactivated())
			{
				lastName = particle.m_name;
				std::string pathLastName = PARTICLE + lastName + EXTENTION;
				particle.m_name = m_bufName;
				std::string pathNewName = PARTICLE + particle.m_name + EXTENTION;
				std::filesystem::rename(pathLastName, pathNewName);
				Logger::Warn("Don't forget to save Particle or it may create a conflict");
			}

			ImGui::Separator();

			ImGui::Checkbox("Diffusal", &particle.diffusal);
			if (ImGui::IsItemDeactivated())
			{
				ParticlesSystem particleSystem(&game->engine->resourceManager, particle);
				engine->preview.ShowParticles(particleSystem);
			}

			if (particle.diffusal)
			{
				ImGui::SliderFloat3("Diffusal direction", &particle.diffusalDirection.x, -1.f, 1.f);
				if (ImGui::IsItemDeactivated())
				{
					ParticlesSystem particleSystem(&game->engine->resourceManager, particle);
					engine->preview.ShowParticles(particleSystem);
				}
				ImGui::SliderFloat3("Diffusal angle", &particle.diffusalAngle.x, 0.f, 2 * Math::PI);
				if (ImGui::IsItemDeactivated())
				{
					ParticlesSystem particleSystem(&game->engine->resourceManager, particle);
					engine->preview.ShowParticles(particleSystem);
				}
			}

			ImGui::SliderInt("Particles number", &particle.particleNumber, 1.f, 1000.f);
			if (ImGui::IsItemDeactivated())
			{
				ParticlesSystem particleSystem(&game->engine->resourceManager, particle);
				engine->preview.ShowParticles(particleSystem);
			}

			ImGui::Separator();

			strncpy(m_bufTexturePath, particle.pathTexture.c_str(), sizeof(m_bufTexturePath) - 1);
			ImGui::InputText("Texture", m_bufTexturePath, IM_ARRAYSIZE(m_bufTexturePath));
			particle.pathTexture = m_bufTexturePath;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
				{
					particle.pathTexture = ASSETS + std::string((const char*)payload->Data);

					ParticlesSystem particleSystem(&game->engine->resourceManager, particle);
					engine->preview.ShowParticles(particleSystem);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Separator();

			if (ImGui::Button("Save Particle"))
			{
				particle.Save();
			}

			if (ImGui::Button("Delete particle"))
			{
				Logger::Info("Delete " + particle.m_name);

				std::string path = PARTICLE + particle.m_name + EXTENTION;
				std::filesystem::remove(path);

				engine->preview.hide = true;
				it = game->m_particlesTemplate.erase(it);

				int index = 0;
				for (ParticlesSystemDefinition& nparticle : game->m_particlesTemplate)
				{
					nparticle.m_id = index;
					index += 1;
				}
				game->m_numberIdParticles = index;
			}
			else
			{
				it++;
			}
		}
		else
		{
			it++;
		}
	}

	ImGui::End();
}