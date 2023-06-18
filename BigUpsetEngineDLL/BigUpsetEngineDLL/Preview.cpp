#include "Preview.h"
#include "Define.h"


Preview::Preview(Renderer* _renderer, ResourceManager* _resourceManager)
	: renderer(_renderer)
	, resourceManager(_resourceManager)
{
	// Set camera parameters

	particlesSystem = ParticlesSystem(resourceManager, ParticlesSystemDefinition(), { 0.f, 0.f, 0.f });

	grid.mesh = _resourceManager->AddMesh(QUAD);
	grid.texture = _resourceManager->AddTexture(DEFAULT_TEXT);
}

void Preview::UpdateAndRender(const float2& windowSize)
{
	camera.Update(windowSize.x, windowSize.y);

	renderer->StartDraw(camera.projMat, camera.viewMat, renderer->previewFBO, windowSize, { 0.3f, 0.3f, 0.3f, 1.f });

	// Draw Unit
	if (!hide)
	{
		if (particles)
		{
			// Show Particles
			for (auto& particle : particlesSystem.particles)
			{
				if (particle.life > 0.f)
				{
					// TODO check Math
					mat4 modelMatrix = Mat4::CreateTRS(particle.position, { Math::PI/2.f, Math::PI / 2.f, 0.f }, particle.scale);
					renderer->DrawObject(modelMatrix, particlesSystem.model);
				}
			}
			particlesSystem.Update(myTimer.deltaTime, 25);
		}
		else
		{
			// show Model
			renderer->DrawObject(Mat4::Identity(), model);
			// Draw Map
			{
				renderer->DrawObject(Mat4::Identity(), grid);
			}
		}
	}

	renderer->EndDraw();
}