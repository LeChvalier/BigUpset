#pragma once

class Preview;

class ScenePanel
{
public:
	int gizmoType = -1;

	//Render ImGui Preview selected entity
	void ImguiRender(Preview* preview);
};