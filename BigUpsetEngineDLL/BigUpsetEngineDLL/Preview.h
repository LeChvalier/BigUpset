#pragma once

#include "TemplateManager.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Entity.h"
#include "Camera.h"
#include "ParticlesSystem.h"


class Preview
{
public:
	Preview(Renderer* _renderer, ResourceManager* _resourceManager);
	~Preview() = default;

	void ShowModel(Model& _model) { model = _model; hide = false; particles = false; };
	void ShowParticles(ParticlesSystem& _particlesSystem) { particlesSystem = _particlesSystem; hide = false; particles = true;	};

	void UpdateAndRender(const float2& windowSize = { 1200.f, 800.f });

	const unsigned int GetCBO() const { return renderer->previewCBO; };

	// Show model in preview or not
	bool hide = true;

	// If there model or particles
	bool particles = false;

	// for particlesSystem update
	Timer myTimer;

private:

	// The only one
	Camera camera;

	// The two objects to draw
	Model model;
	ParticlesSystem particlesSystem;
	Model grid;

	// Contains all the resources
	ResourceManager* resourceManager = nullptr;

	// Renders the scene
	Renderer* renderer;
};