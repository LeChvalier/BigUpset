#pragma once

struct float2;
struct float3;
struct mat4;

inline bool DEBUGMODE = false;

void DrawGizmo(const float2& windowSize, const float3& center, const mat4& viewProj);

void DrawSegement(const float2& windowSize, const mat4& viewProj, const float3& p0, const float3& p1);

// Draw quad on the ground
void DrawQuad(const float2& windowSize, const float3& center, const mat4& viewProj, const float2& quadSize);

// Draw unit range
void DrawRange(const float2& windowSize, const float3& center, const mat4& viewProj, const float range, const int rayNumber = 30);