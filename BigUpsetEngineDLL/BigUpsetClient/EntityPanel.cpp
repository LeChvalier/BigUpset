#include <imgui.h>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

#include "BigUpsetEngine.h"
#include "GameDLL.h"

#include "Editor.h"
#include "TemplateManager.h"
#include "EntityPanel.h"
#include "Logger.h"

#include <algorithm>

#include "Define.h"


EntityPanel::EntityPanel()
{
}

EntityPanel::~EntityPanel()
{
}

void EntityPanel::ListBoxUnit(const char* name, std::list<UnitDefinition>& units, int& selectedUnitID, Engine* engine, Selected& selected)
{
    if (ImGui::ListBoxHeader(name, { thumbnailSize, (thumbnailSize / 6) * units.size() + 20 }))
    {
        int j = 0;
        for (UnitDefinition& unit : units)
        {
            ImGui::PushID(j);

            const std::string& item_name = unit.m_name;

            if (ImGui::Selectable(item_name.c_str(), j == selectedUnitID, 0, { thumbnailSize, thumbnailSize / 6 }))
            {
                if (unit.m_type == UT_BUILDING)
                    buildingSelectedId = j;
                else if (unit.m_type == UT_CHARACTER)
                    characterSelectedId = j;

                if (unit.m_id == selectedUnitID)
                    engine->preview.ShowModel(unit.model);

                if (unit.m_type == UT_BUILDING)
                    selected = Selected::E_Building;
                else if (unit.m_type == UT_CHARACTER)
                    selected = Selected::E_Character;

                if (unit.m_type == UT_BUILDING)
                    characterSelectedId = -1;
                else if (unit.m_type == UT_CHARACTER)
                    buildingSelectedId = -1;

                decorSelectedId = -1;

            }
            j++;
            ImGui::PopID();
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ENTITY_ID", &unit.m_id, sizeof(int));
                ImGui::EndDragDropSource();
            }
        }
        ImGui::EndListBox();
    }
}

void EntityPanel::ListBoxDecor(const char* name, std::list<DecorDefinition>& decors, int& selectedUnitID, Engine* engine, Selected& selected)
{
    
    if (ImGui::ListBoxHeader(name, { thumbnailSize, (thumbnailSize / 6) * decors.size() + 20 }))
    {
        int j = 0;
        for (DecorDefinition& decor : decors)
        {
            ImGui::PushID(j);

            const std::string& item_name = decor.m_name;

            if (ImGui::Selectable(item_name.c_str(), j == selectedUnitID, 0, { thumbnailSize, thumbnailSize / 6 }))
            {
                decorSelectedId = j;

                if (decor.m_id == selectedUnitID)
                {
                    engine->preview.ShowModel(decor.model);

                    //engine->scene.CleanScene();
                    //engine->scene.NewDecor(decor, { 0,0,0 });
                }

                selected = Selected::E_Decor;
                
                characterSelectedId = -1;
                buildingSelectedId = -1;
            }
            j++;
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }

}

void EntityPanel::ImguiRender(Game* game, Engine* engine, Selected& selected)
{
    if (selected != Selected::E_Building && selected != Selected::E_Character && selected != Selected::E_Decor)
    {
        buildingSelectedId = -1;
        characterSelectedId = -1;
        decorSelectedId = -1;
    }

    if (ImGui::Begin("Entities"))
    {
        ImGui::Separator();
        ListBoxUnit("Characters :", game->m_charactersTemplate, characterSelectedId, engine, selected);

        ImGui::Separator();

        if (ImGui::Button("New Character", { thumbnailSize, thumbnailSize / 6 }))
        {
            bool sameName = false;
            for (UnitDefinition& chara : game->m_charactersTemplate)
            {
                if (chara.m_name == "Default name")
                    sameName = true;
            }
            if (sameName)
                Logger::Error("Default name already exist, rename it and save it for create new characters");
            else
            {
                game->newCharacterTemplate();
                characterSelectedId = game->m_charactersTemplate.size() - 1;
                engine->preview.ShowModel(game->m_charactersTemplate.back().model);
            }
        }

        ImGui::Separator();
        ImGui::Separator();

        ListBoxUnit("Buildings :", game->m_buildingsTemplate, buildingSelectedId, engine, selected);

        ImGui::Separator();

        if (ImGui::Button("New Building", { thumbnailSize, thumbnailSize / 6 }))
        { 
            bool sameName = false;
            for (UnitDefinition& build : game->m_buildingsTemplate)
            {
                if (build.m_name == "Default name")
                    sameName = true;
            }
            if (sameName)
                Logger::Error("Default name already exist, rename it and save it for create new buildings");
            else
            {
                game->newBuildingsTemplate();
                buildingSelectedId = game->m_buildingsTemplate.size() - 1;
                engine->preview.ShowModel(game->m_buildingsTemplate.back().model);
            }
        }

        ImGui::Separator();
        ImGui::Separator();

        ListBoxDecor("Decors :", game->m_decorsTemplate, decorSelectedId, engine, selected);

        ImGui::Separator();

        if (ImGui::Button("New Decor", { thumbnailSize, thumbnailSize / 6 }))
        {
            bool sameName = false;
            for (DecorDefinition& decor : game->m_decorsTemplate)
            {
                if (decor.m_name == "Default name")
                    sameName = true;
            }
            if (sameName)
                Logger::Error("Default name already exist, rename it and save it for create new decors");
            else
            {
                game->newDecorTemplate();
                decorSelectedId = game->m_decorsTemplate.size() - 1;
                engine->preview.ShowModel(game->m_decorsTemplate.back().model);
            }
               
        }

    }
    ImGui::End();

    ImguiRenderComponents(game, engine, selected);
}

#pragma warning(disable : 4996)
void EntityPanel::ImguiRenderComponents(Game* game, Engine* engine, Selected& selected)
{
    assert(game != nullptr);

    ImGui::Begin("Components");

    if(selected == Selected::E_Character)
    {
        static std::string lastNameChara;
        static std::string lastShotSound;

        for (auto it = game->m_charactersTemplate.begin(); it != game->m_charactersTemplate.end();)
        {
            UnitDefinition& chara = *it;

            if (chara.m_id == characterSelectedId)
            {
                strncpy(bufName, chara.m_name.c_str(), sizeof(bufName) - 1);

                ImGui::InputText("name", bufName, IM_ARRAYSIZE(bufName));
                if (ImGui::IsItemDeactivated())
                {
                    lastNameChara = chara.m_name;
                    std::string pathLastName = CHARACTER + lastNameChara + EXTENTION;
                    chara.m_name = bufName;
                    std::string pathNewName = CHARACTER + chara.m_name + EXTENTION;
                    std::filesystem::rename(pathLastName, pathNewName);
                    Logger::Warn("Don't forget to save Entity or it may create a conflict");
                }

                ImGui::Text("Side :");

                if (ImGui::BeginCombo("Side", ScriptHelper::GetNameSide(chara.m_side).c_str()))
                {
                    for (int i = 0; i < (int)COUNTER; i++)
                    {
                        if (ImGui::Selectable(ScriptHelper::GetNameSide((AISideType)i).c_str()))
                        {
                            chara.m_side = (AISideType)i;
                            m_sideSelected = false;
                            Logger::Warn("Don't forget to save entity");
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Text("Script :");
                
                if (ImGui::BeginCombo("Script", ScriptHelper::GetNameScript(chara.scriptType).c_str()))
                {
                    for (int i = 0; i < (int)COUNT; i++)
                    {
                        if (ImGui::Selectable(ScriptHelper::GetNameScript((ScriptType)i).c_str()))
                        {
                            chara.scriptType = (ScriptType)i;
                            m_scriptSelected = false;
                            Logger::Warn("Don't forget to save entity");
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Separator();

                ImGui::InputInt("Life", &chara.m_maxlife);

                if (chara.m_side == AISideType::ALLIED)
                {
                    ImGui::InputInt("Price", &chara.m_price);
                }
                else if (chara.m_side == AISideType::ENEMY)
                {
                    ImGui::InputInt("Reward", &chara.m_reward);
                }

                ImGui::Checkbox("Can Attack", &chara.m_canAttack);

                if (chara.m_canAttack)
                {
                    ImGui::InputInt("Damage", &chara.m_damage);
                    ImGui::InputInt("Attack speed", &chara.m_attackSpeed);
                    ImGui::InputInt("Attack range", &chara.m_attackRange);
                }

                ImGui::InputInt("Speed", &chara.m_speed);

                ImGui::Separator();

                strncpy(bufMeshPath, chara.pathMesh.c_str(), sizeof(bufMeshPath) - 1);
                ImGui::InputText("Mesh", bufMeshPath, IM_ARRAYSIZE(bufMeshPath));
                chara.pathMesh = bufMeshPath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_MESH"))
                    {
                        chara.pathMesh = ASSETS + std::string((const char*)payload->Data);
                        chara.model.mesh = game->engine->resourceManager.AddMesh(chara.pathMesh.c_str());
                        engine->preview.ShowModel(chara.model);
                    }
                    ImGui::EndDragDropTarget();
                }

                strncpy(bufTexturePath, chara.pathTexture.c_str(), sizeof(bufTexturePath) - 1);
                ImGui::InputText("Texture", bufTexturePath, IM_ARRAYSIZE(bufTexturePath));
                chara.pathTexture = bufTexturePath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
                    {
                        chara.pathTexture = ASSETS + std::string((const char*)payload->Data);
                        chara.model.texture = game->engine->resourceManager.AddTexture(chara.pathTexture.c_str());
                        engine->preview.ShowModel(chara.model);

                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Separator();

                ImGui::Text("Sound effects : ");
                ImGui::Separator();

                if (chara.m_canAttack)
                {
                    strncpy(bufShotSoundPath, chara.m_shotSound.c_str(), sizeof(bufShotSoundPath) - 1);
                    ImGui::InputText("Shot", bufShotSoundPath, IM_ARRAYSIZE(bufShotSoundPath));
                    chara.m_shotSound = bufShotSoundPath;

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_SOUND"))
                        {
                            chara.m_shotSound = ASSETS + std::string((const char*)payload->Data);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                strncpy(bufDieSoundPath, chara.m_dieSound.c_str(), sizeof(bufDieSoundPath) - 1);
                ImGui::InputText("Die", bufDieSoundPath, IM_ARRAYSIZE(bufDieSoundPath));
                chara.m_dieSound = bufDieSoundPath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_SOUND"))
                    {
                        chara.m_dieSound = ASSETS + std::string((const char*)payload->Data);
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Text("Particles effects : ");
                ImGui::Separator();

                std::string actualAttackParticle = "";
                for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                {
                    if (particle.m_id == chara.m_IdAttackParticle)
                    {
                        actualAttackParticle = particle.m_name;
                    }
                }
                
                if (ImGui::BeginCombo("Attack Particle", actualAttackParticle.c_str()))
                {
                    for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                    {
                        if (ImGui::Selectable(particle.m_name.c_str()))
                        {
                            chara.m_IdAttackParticle = particle.m_id;
                        }
                    }

                    ImGui::EndCombo();
                }
                /*if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID"))
                    {
                        chara.m_IdUnitDefCreateable = *(const int*)payload->Data;
                    }
                    ImGui::EndDragDropTarget();
                }*/

                std::string actualDieParticle = "";
                for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                {
                    if (particle.m_id == chara.m_IdDieParticle)
                    {
                        actualDieParticle = particle.m_name;
                    }
                }

                if (ImGui::BeginCombo("Die Particle", actualDieParticle.c_str()))
                {
                    for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                    {
                        if (ImGui::Selectable(particle.m_name.c_str()))
                        {
                            chara.m_IdDieParticle = particle.m_id;
                        }
                    }

                    ImGui::EndCombo();
                }
                /*if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID"))
                    {
                        chara.m_IdUnitDefCreateable = *(const int*)payload->Data;
                    }
                    ImGui::EndDragDropTarget();
                }*/

                ImGui::Separator();

                if (ImGui::Button("Refresh Character"))
                {
                    chara.model.mesh = game->engine->resourceManager.AddMesh(chara.pathMesh.c_str());
                    chara.model.texture = game->engine->resourceManager.AddTexture(chara.pathTexture.c_str());
                    engine->preview.ShowModel(chara.model);
                }

                if (ImGui::Button("Save Character"))
                {
                    chara.Save();
                }

                if (ImGui::Button("Delete character"))
                {
                    Logger::Info("Delete " + chara.m_name);

                    std::string path = CHARACTER + chara.m_name + EXTENTION;
                    std::filesystem::remove(path);

                    engine->preview.hide = true;
                    it = game->m_charactersTemplate.erase(it);

                    int index = 0;
                    for (UnitDefinition& nchara : game->m_charactersTemplate)
                    {
                        nchara.m_id = index;
                        index += 1;
                    }
                    game->m_numberIdCharacter = index;
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
    }
    if (selected == Selected::E_Building)
    {
        static std::string lastNameBuild;

        for (auto it = game->m_buildingsTemplate.begin(); it != game->m_buildingsTemplate.end();)
        {
            UnitDefinition& build = *it;

            if (build.m_id == buildingSelectedId)
            {
                strncpy(bufName, build.m_name.c_str(), sizeof(bufName) - 1);

                ImGui::InputText("name", bufName, IM_ARRAYSIZE(bufName));
                if (ImGui::IsItemDeactivated())
                {
                    lastNameBuild = build.m_name;
                    std::string pathLastName = BUILDING + lastNameBuild + EXTENTION;
                    build.m_name = bufName;
                    std::string pathNewName = BUILDING + build.m_name + EXTENTION;
                    std::filesystem::rename(pathLastName, pathNewName);
                    Logger::Warn("Don't forget to save Entity or it may create a conflict");
                }

                ImGui::Separator();

                ImGui::InputInt("Life", &build.m_maxlife);

                if (build.m_side == AISideType::ALLIED)
                {
                    ImGui::InputInt("Price", &build.m_price);
                }
                else if (build.m_side == AISideType::ENEMY)
                {
                    ImGui::InputInt("Reward", &build.m_reward);
                }

                ImGui::Checkbox("Can Attack", &build.m_canAttack);

                if (build.m_canAttack)
                {
                    ImGui::InputInt("Damage", &build.m_damage);
                    ImGui::InputInt("Attack speed", &build.m_attackSpeed);
                    ImGui::InputInt("Attack range", &build.m_attackRange);
                }

                ImGui::Checkbox("Can create character", &build.m_canCreateCharacter);
                ImGui::Checkbox("Main Building", &build.m_mainBuilding);

                if (build.m_canCreateCharacter)
                {
                    ImGui::InputInt("Cooldown create character", &build.m_cooldownCreateCharacter);

                    std::string actualunit = "";
                    for (UnitDefinition& unit : game->m_charactersTemplate)
                    {
                        if (unit.m_id == build.m_IdUnitDefCreateable)
                        {
                            actualunit = unit.m_name;
                        }
                    }

                    if (build.m_canCreateCharacter)
                    {
                        if (ImGui::BeginCombo("Unit creatable", actualunit.c_str()))
                        {
                            for (UnitDefinition& unit : game->m_charactersTemplate)
                            {
                                if (ImGui::Selectable(unit.m_name.c_str()))
                                {
                                    build.m_IdUnitDefCreateable = unit.m_id;
                                }
                            }

                            ImGui::EndCombo();
                        }
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID"))
                            {
                                build.m_IdUnitDefCreateable = *(const int*)payload->Data;
                            }
                            ImGui::EndDragDropTarget();
                        }
                    }
                }


                ImGui::Separator();

                strncpy(bufMeshPath, build.pathMesh.c_str(), sizeof(bufMeshPath) - 1);
                ImGui::InputText("Mesh", bufMeshPath, IM_ARRAYSIZE(bufMeshPath));
                build.pathMesh = bufMeshPath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_MESH"))
                    {
                        build.pathMesh = ASSETS + std::string((const char*)payload->Data);
                        build.model.mesh = game->engine->resourceManager.AddMesh(build.pathMesh.c_str());
                        engine->preview.ShowModel(build.model);
                    }
                    ImGui::EndDragDropTarget();
                }

                strncpy(bufTexturePath, build.pathTexture.c_str(), sizeof(bufTexturePath) - 1);
                ImGui::InputText("Texture", bufTexturePath, IM_ARRAYSIZE(bufTexturePath));
                build.pathTexture = bufTexturePath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
                    {
                        build.pathTexture = ASSETS + std::string((const char*)payload->Data);
                        build.model.texture = game->engine->resourceManager.AddTexture(build.pathTexture.c_str());
                        engine->preview.ShowModel(build.model);

                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Text("SOUND EFFECTS");

                if (build.m_canAttack)
                {
                    strncpy(bufShotSoundPath, build.m_shotSound.c_str(), sizeof(bufShotSoundPath) - 1);
                    ImGui::InputText("Shot", bufShotSoundPath, IM_ARRAYSIZE(bufShotSoundPath));
                    build.m_shotSound = bufShotSoundPath;

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_SOUND"))
                        {
                            build.m_shotSound = ASSETS + std::string((const char*)payload->Data);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                strncpy(bufDieSoundPath, build.m_dieSound.c_str(), sizeof(bufDieSoundPath) - 1);
                ImGui::InputText("Die", bufDieSoundPath, IM_ARRAYSIZE(bufDieSoundPath));
                build.m_dieSound = bufDieSoundPath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_SOUND"))
                    {
                        build.m_dieSound = ASSETS + std::string((const char*)payload->Data);
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Separator();

                if (ImGui::Button("Refresh Building"))
                {
                    build.model.mesh = game->engine->resourceManager.AddMesh(build.pathMesh.c_str());
                    build.model.texture = game->engine->resourceManager.AddTexture(build.pathTexture.c_str());
                    engine->preview.ShowModel(build.model);

                }

                ImGui::Separator();

                ImGui::Text("Side :");

                if (ImGui::BeginCombo("Side", ScriptHelper::GetNameSide(build.m_side).c_str()))
                {
                    for (int i = 0; i < (int)COUNTER; i++)
                    {
                        if (ImGui::Selectable(ScriptHelper::GetNameSide((AISideType)i).c_str()))
                        {
                            build.m_side = (AISideType)i;
                            m_sideSelected = false;
                            Logger::Warn("Don't forget to save entity");
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Separator();

                ImGui::Text("Script :");

                if (ImGui::BeginCombo("Script", ScriptHelper::GetNameScript(build.scriptType).c_str()))
                {
                    for (int i = 0; i < (int)COUNT; i++)
                    {
                        if (ImGui::Selectable(ScriptHelper::GetNameScript((ScriptType)i).c_str()))
                        {
                            build.scriptType = (ScriptType)i;
                            m_scriptSelected = false;
                            Logger::Warn("Don't forget to save entity");
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Text("Particles effects : ");
                ImGui::Separator();

                std::string actualAttackParticle = "";
                for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                {
                    if (particle.m_id == build.m_IdAttackParticle)
                    {
                        actualAttackParticle = particle.m_name;
                    }
                }

                if (ImGui::BeginCombo("Attack Particle", actualAttackParticle.c_str()))
                {
                    for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                    {
                        if (ImGui::Selectable(particle.m_name.c_str()))
                        {
                            build.m_IdAttackParticle = particle.m_id;
                        }
                    }

                    ImGui::EndCombo();
                }
                /*if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID"))
                    {
                        chara.m_IdUnitDefCreateable = *(const int*)payload->Data;
                    }
                    ImGui::EndDragDropTarget();
                }*/

                std::string actualDieParticle = "";
                for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                {
                    if (particle.m_id == build.m_IdDieParticle)
                    {
                        actualDieParticle = particle.m_name;
                    }
                }

                if (ImGui::BeginCombo("Die Particle", actualDieParticle.c_str()))
                {
                    for (ParticlesSystemDefinition& particle : game->m_particlesTemplate)
                    {
                        if (ImGui::Selectable(particle.m_name.c_str()))
                        {
                            build.m_IdDieParticle = particle.m_id;
                        }
                    }

                    ImGui::EndCombo();
                }
                /*if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID"))
                    {
                        chara.m_IdUnitDefCreateable = *(const int*)payload->Data;
                    }
                    ImGui::EndDragDropTarget();
                }*/

                ImGui::Separator();

                if (ImGui::Button("Save Building"))
                {
                    build.Save();
                }

                ImGui::Separator();

                if (ImGui::Button("Delete building"))
                {
                    Logger::Info("Delete " + build.m_name);
                    
                    std::string path = BUILDING + build.m_name + EXTENTION;
                    std::filesystem::remove(path);
                    
                    engine->preview.hide = true;

                    it = game->m_buildingsTemplate.erase(it);
                    
                    int index = 0;
                    for (UnitDefinition& nbuild : game->m_buildingsTemplate)
                    {
                        nbuild.m_id = index;
                        index += 1;
                    }
                    game->m_numberIdBuildings = index;
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
    }

    if (selected == Selected::E_Decor)
    {
        static std::string lastNameDecor;

        for (auto it = game->m_decorsTemplate.begin(); it != game->m_decorsTemplate.end();)
        {
            DecorDefinition& decor = *it;

            if (decor.m_id == decorSelectedId)
            {
                strncpy(bufName, decor.m_name.c_str(), sizeof(bufName) - 1);

                ImGui::InputText("name", bufName, IM_ARRAYSIZE(bufName));
                if (ImGui::IsItemDeactivated())
                {
                    lastNameDecor = decor.m_name;
                    std::string pathLastName = DECOR + lastNameDecor + EXTENTION;
                    decor.m_name = bufName;
                    std::string pathNewName = DECOR + decor.m_name + EXTENTION;
                    std::filesystem::rename(pathLastName, pathNewName);
                    Logger::Warn("Don't forget to save Entity or it may create a conflict");
                }

                ImGui::Separator();

                strncpy(bufMeshPath, decor.pathMesh.c_str(), sizeof(bufMeshPath) - 1);
                ImGui::InputText("Mesh", bufMeshPath, IM_ARRAYSIZE(bufMeshPath));
                decor.pathMesh = bufMeshPath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_MESH"))
                    {
                        decor.pathMesh = ASSETS + std::string((const char*)payload->Data);
                        decor.model.mesh = game->engine->resourceManager.AddMesh(decor.pathMesh.c_str());
                        engine->preview.ShowModel(decor.model);

                    }
                    ImGui::EndDragDropTarget();
                }

                strncpy(bufTexturePath, decor.pathTexture.c_str(), sizeof(bufTexturePath) - 1);
                ImGui::InputText("Texture", bufTexturePath, IM_ARRAYSIZE(bufTexturePath));
                decor.pathTexture = bufTexturePath;

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
                    {
                        decor.pathTexture = ASSETS + std::string((const char*)payload->Data);
                        decor.model.texture = game->engine->resourceManager.AddTexture(decor.pathTexture.c_str());
                        engine->preview.ShowModel(decor.model);

                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::Button("Refresh Decor"))
                {
                    decor.model.mesh = game->engine->resourceManager.AddMesh(decor.pathMesh.c_str());
                    decor.model.texture = game->engine->resourceManager.AddTexture(decor.pathTexture.c_str());
                    engine->preview.ShowModel(decor.model);

                }

                ImGui::Separator();

                if (ImGui::Button("Save Decor"))
                {
                    decor.Save();
                }

                ImGui::Separator();

                if (ImGui::Button("Delete decor"))
                {
                    Logger::Info("Delete " + decor.m_name);

                    std::string path = DECOR + decor.m_name + EXTENTION;
                    std::filesystem::remove(path);

                    engine->preview.hide = true;

                    it = game->m_decorsTemplate.erase(it);

                    int index = 0;
                    for (DecorDefinition& ndecor : game->m_decorsTemplate)
                    {
                        ndecor.m_id = index;
                        index += 1;
                    }
                    game->m_numberIdDecors = index;
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
    }

    ImGui::End();
}