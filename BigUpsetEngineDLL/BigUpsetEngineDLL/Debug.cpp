#include <imgui.h>

#include "Debug.h"
#include "Maths.h"

#include "Define.h"

#include <iostream>
#include <vector>

void DrawGizmo(const float2& windowSize, const float3& center, const mat4& viewProj)
{
	if (!DEBUGMODE)
		return;

	// Why ImGui::GetForegroundDrawList() is broken when engine runs
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	//ImDrawList* drawList = ImGui::GetWindowDrawList();

	float3 A = center + float3{ 1.f, 0.f, 0.f };
	float3 B = center + float3{ 0.f, 1.f, 0.f };
	float3 C = center + float3{ 0.f, 0.f, 1.f };

	ImVec2 o = ImVec2{ Math::WorldToScreen(center, viewProj, windowSize).x, Math::WorldToScreen(center, viewProj, windowSize).y };
	ImVec2 a = ImVec2{ Math::WorldToScreen(A, viewProj, windowSize).x, Math::WorldToScreen(A, viewProj, windowSize).y };
	ImVec2 b = ImVec2{ Math::WorldToScreen(B, viewProj, windowSize).x, Math::WorldToScreen(B, viewProj, windowSize).y };
	ImVec2 c = ImVec2{ Math::WorldToScreen(C, viewProj, windowSize).x, Math::WorldToScreen(C, viewProj, windowSize).y };

	// Points
	drawList->AddCircle(a, 1.f, RED);
	drawList->AddCircle(b, 1.f, GREEN);
	drawList->AddCircle(c, 1.f, BLUE);

	// Lines
	drawList->AddLine(o, a, RED);
	drawList->AddLine(o, b, GREEN);
	drawList->AddLine(o, c, BLUE);
}

void DrawQuad(const float2& windowSize, const float3& center, const mat4& viewProj, const float2& quadSize)
{
	if (!DEBUGMODE)
		return;

	// To render a tile (WIP)
	/*float2 tileSize = { map.scale.x / map.tilesWidth,  map.scale.z / map.tilesHeight };
	 u and v are the gameplay tile center
	int u = (lastClickedPos.x /* + map.scale.x / 2.f  ) * tileSize.x);
	int v = (lastClickedPos.z /* + map.scale.z / 2.f) * tileSize.y);
	Math::Clamp<int>(u, 0, map.tilesWidth);
	Math::Clamp<int>(v, 0, map.tilesHeight);
	float3 tileCenter = { (float)u, 0.f, (float)v };*/

	// Why ImGui::GetForegroundDrawList() is broken when engine runs
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	//ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Show GamePlay tile
	float3 DownLeft{ center.x + quadSize.x / 2.f, 0.f, center.z + quadSize.y / 2.f };
	float3 DownRight{ center.x + quadSize.x / 2.f, 0.f, center.z - quadSize.y / 2.f };
	float3 UpRight{ center.x - quadSize.x / 2.f, 0.f, center.z - quadSize.y / 2.f };
	float3 UpLeft{ center.x - quadSize.x / 2.f, 0.f, center.z + quadSize.y / 2.f };

	float2 DL = Math::WorldToScreen(DownLeft, viewProj, windowSize);
	float2 DR = Math::WorldToScreen(DownRight, viewProj, windowSize);
	float2 UR = Math::WorldToScreen(UpRight, viewProj, windowSize);
	float2 UL = Math::WorldToScreen(UpLeft, viewProj, windowSize);

	// Lines
	drawList->AddLine({ DL.x, DL.y }, { DR.x, DR.y }, GREEN);
	drawList->AddLine({ DR.x, DR.y }, { UR.x, UR.y }, GREEN);
	drawList->AddLine({ UR.x, UR.y }, { UL.x, UL.y }, GREEN);
	drawList->AddLine({ UL.x, UL.y }, { DL.x, DL.y }, GREEN);
}

void DrawRange(const float2& windowSize, const float3& center, const mat4& viewProj, const float range, const int rayNumber)
{
	if (!DEBUGMODE)
		return;

	// Why ImGui::GetForegroundDrawList() is broken when engine runs
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	//ImDrawList* drawList = ImGui::GetWindowDrawList();


	for (int i = 0; i < rayNumber; i++)
	{
		float3 p0 = {
			center.x + range * cosf((float)i / (float)rayNumber * Math::TAU),
			0.f,
			center.z + range * sinf((float)i / (float)rayNumber * Math::TAU) };

		float3 p1 = {
			center.x + range * cosf((float)(i + 1) / (float)rayNumber * Math::TAU),
			0.f,
			center.z + range * sinf((float)(i + 1) / (float)rayNumber * Math::TAU) };

		float2 screenP0 = Math::WorldToScreen(p0, viewProj, windowSize);
		float2 screenP1 = Math::WorldToScreen(p1, viewProj, windowSize);

		drawList->AddLine({ screenP0.x, screenP0.y }, { screenP1.x, screenP1.y }, GREEN);

	}
}

void DrawSegement(const float2& windowSize, const mat4& viewProj, const float3& p0, const float3& p1)
{
	if (!DEBUGMODE)
		return;

	// Why ImGui::GetForegroundDrawList() is broken when engine runs
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	//ImDrawList* drawList = ImGui::GetWindowDrawList();

	float2 screenP0 = Math::WorldToScreen(p0, viewProj, windowSize);
	float2 screenP1 = Math::WorldToScreen(p1, viewProj, windowSize);

	drawList->AddLine({ screenP0.x, screenP0.y }, { screenP1.x, screenP1.y }, BLUE);
}