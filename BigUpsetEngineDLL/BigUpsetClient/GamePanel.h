#pragma once

class Game;

class GamePanel
{
public:
	GamePanel();
	~GamePanel();

	//Render ImGui Game Window
	void ImguiRender(Game* game, float2& GameWindowSize, float2& gameScreenPos);
private:
};