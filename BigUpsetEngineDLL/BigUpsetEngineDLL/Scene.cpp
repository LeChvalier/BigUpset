#include <iostream>

#include "ObjectPointer.h"
#include "Entity.h"
#include "Logger.h"
#include "Scene.h"


void Scene::CleanScene()
{
	// Remove all the scene objects
	buildingEntities.clear();
	characterEntities.clear();
	decorEntities.clear();
	particlesSystems.clear();
}

void Scene::NewUnit(UnitDefinition& definition, float3 pos)
{
	UnitEntity newUnit(definition);
	newUnit.position = pos;
	newUnit.definition.m_life = newUnit.definition.m_maxlife;
	newUnit.gameID = IDcount;
	IDcount++;
	if (definition.m_type == UT_BUILDING)
		buildingEntities.push_back(newUnit);
	else if (definition.m_type == UT_CHARACTER)
		characterEntities.push_back(newUnit);
}

void Scene::NewDecor(DecorDefinition& definition, float3 pos, float3 rotation)
{
	DecorEntity newDecor(definition);
	newDecor.position = pos;
	newDecor.rotation = rotation;
	decorEntities.push_back(newDecor);
}

void Scene::NewParticlesSystem(ParticlesSystemDefinition def, const float3& _position, const float3& _rotation)
{
	particlesSystems.push_back(ParticlesSystem(resourceManager, def, _position, _rotation));
}

Scene::Scene(Renderer* _renderer, ResourceManager* _resourceManager)
	: renderer(_renderer)
	, resourceManager(_resourceManager)
{

}

void Scene::UpdateAndRender(std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, const bool& inGame, const float2& windowSize, EconomySystem& economySystem)
{
	camera.Update(windowSize.x, windowSize.y);

	Timer myTimer;

	renderer->StartDraw(camera.projMat, camera.viewMat, renderer->FBO, windowSize);

	// Draw map
	{
		mat4 modelMatrix = Mat4::CreateTRS(map.position, map.rotation, map.scale);

		if (!map.initialized)
		{
			// Always a quad
			map.model.mesh = resourceManager->AddMesh(QUAD);

			map.model.texture = resourceManager->AddTexture(map.model.texture->path.c_str());
			map.tiles = resourceManager->ParseMap(map.gameplayMap, map.tilesWidth, map.tilesHeight);

			map.initialized = true;
		}

		renderer->DrawObject(modelMatrix, map.model);
	}

	// Draw decor elements
	for (DecorEntity& decor : decorEntities)
	{
		mat4 modelMatrix = Mat4::CreateTRS(decor.position, decor.rotation, decor.scale);
		renderer->DrawObject(modelMatrix, decor.definition->model);
	}

	for (UnitEntity& entity : characterEntities)
	{
		mat4 modelMatrix = Mat4::CreateTRS(entity.position, entity.rotation, entity.scale);
		renderer->DrawObject(modelMatrix, entity.definition.model);
		if (inGame)
			entity.script->Update(entity, this, CharaDefinitions, BuildDefinitions, soundManager, economySystem);
	}
	for (UnitEntity& entity : buildingEntities)
	{
		mat4 modelMatrix = Mat4::CreateTRS(entity.position, entity.rotation, entity.scale);
		renderer->DrawObject(modelMatrix, entity.definition.model);
		if (inGame)
			entity.script->Update(entity, this, CharaDefinitions, BuildDefinitions, soundManager, economySystem);
	}
	int i = 0;
	for (auto& particlesSystem : particlesSystems)
	{
		if (particlesSystem.life < 0.f)
		{
			particlesSystems.erase(particlesSystems.begin() + i);
			break;
		}
		for (auto& particle : particlesSystem.particles)
		{
			if (particle.life > 0.f)
			{
				// TODO check Math
				mat4 modelMatrix = Mat4::CreateTRS(particle.position, { camera.yaw, camera.pitch, camera.roll }, particle.scale);
				renderer->DrawObject(modelMatrix, particlesSystem.model);
			}
		}
		particlesSystem.Update(myTimer.deltaTime, 25);
		i++;
	}

	renderer->EndDraw();
}
