#include "ParticlesSystem.h"
#include "ShaderLoader.h"

#include <glad/gl.h>

#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "ResourceManager.h"
#include "Entity.h"
#include <iostream>

ParticlesSystem::ParticlesSystem(ResourceManager* resourceManager, ParticlesSystemDefinition def, const float3& _position, const float3& _rotation)
{
	model.texture = resourceManager->AddTexture(def.pathTexture.c_str());
	model.mesh = resourceManager->AddMesh(QUAD);

	// create amount default particle instances
	for (unsigned int i = 0; i < def.particleNumber; ++i)
		particles.push_back(Particle3D());

	position = _position;
	diffusal = def.diffusal;

	//diffusalDirection = def.diffusalDirection;
	diffusalDirection = _rotation;
	diffusalAngle = def.diffusalAngle;
}

void ParticlesSystem::RespawnParticle(Particle3D& particle)
{
	// Reused particles
	particle.position = position;
	particle.life = particle.maxLife;

	if (diffusal)
	{
		particle.velocity.x = diffusalDirection.x + diffusalAngle.x * (((rand() % 100) - 50.f) / 50.f * cosf(diffusalDirection.x));
		particle.velocity.y = diffusalDirection.y + diffusalAngle.y * (((rand() % 100) - 50.f) / 50.f * sinf(diffusalDirection.y));
		particle.velocity.z = diffusalDirection.z + diffusalAngle.z * (((rand() % 100) - 50.f) / 50.f * cosf(diffusalDirection.z));
	}
	else
		particle.velocity = { ((rand() % 100) - 50.f) / 100.f, ((rand() % 100) - 50.f) / 100.f, ((rand() % 100) - 50.f) / 100.f };
}

unsigned int ParticlesSystem::FirstUnusedParticle()
{
	// search from last used particle, this will usually return almost instantly
	for (unsigned int i = m_lastUsedParticle; i < particles.size() - m_lastUsedParticle; ++i)
		if (particles[i].life <= 0.0f)
		{
			m_lastUsedParticle = i;
			return i;
		}

	// otherwise, do a linear search
	for (unsigned int i = 0; i < m_lastUsedParticle; ++i)
	{
		if (particles[i].life <= 0.0f)
		{
			m_lastUsedParticle = i;
			return i;
		}
	}

	/*for (unsigned int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].life <= 0.0f)
		{
			m_lastUsedParticle = i;
			return i;
		}
	}*/

	// override first particle if all others are alive
	m_lastUsedParticle = 0;
	return -1;
}

void ParticlesSystem::Update(float dt, unsigned int newParticles)
{
	// Add new particles
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = FirstUnusedParticle();
		if (unusedParticle != -1)
			RespawnParticle(particles[unusedParticle]);
	}

	// Update all particles
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		Particle3D& p = particles[i];
		p.life -= dt;
		if (p.life > 0.0f)
		{
			p.position = p.position + (p.velocity * dt);
			//p.Color.r -= dt * 2.5f;
		}
	}
	life -= dt;
}