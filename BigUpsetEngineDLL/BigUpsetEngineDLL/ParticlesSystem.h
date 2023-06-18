#pragma once

#include "Maths.h"
#include "Define.h"

#include <vector>
#include "Objects.h"

class ResourceManager;
struct ParticlesSystemDefinition;

struct Particle3D
{
	float3 scale = { 0.1f, 0.1f, 0.1f };
	float3 position = { 0.f, 0.f, 0.f };
	float3 rotation = { 0.f, 0.f, 0.f };
	float3 velocity = { 0.f, 0.f, 0.f };
	//float4 Color = { 1.f, 0.f, 0.f, 0.f };

	float maxLife = 1.f;
	float life = 0.f;
};

class ParticlesSystem
{
public:
	ParticlesSystem() {};
	ParticlesSystem(ResourceManager* resourceManager, ParticlesSystemDefinition def, const float3& _position = { 0.f, 0.f, 0.f }, const float3& _rotation = {0.f, 0.f, 0.f});
	~ParticlesSystem() {};

	void Update(float dt, unsigned int newParticles);

	// Commun model of all the particles
	Model model;
	float3 position = { 0.f, 0.f, 0.f };
	// rotation?
	// scales?

	// Particles
	std::vector<Particle3D> particles;

	bool diffusal = false;
	float3 diffusalDirection = { 1.f, 0.f, 0.f };
	float3 diffusalAngle = { Math::PI / 4.f, Math::PI / 4.f, Math::PI / 4.f };

	float life = 1.f;
private:

	void RespawnParticle(Particle3D& particle);
	unsigned int FirstUnusedParticle();

	unsigned int m_lastUsedParticle = 0;

};