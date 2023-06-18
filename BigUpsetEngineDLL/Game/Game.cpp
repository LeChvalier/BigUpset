#include <iostream>

#include "BigUpsetEngine.h"
#include "GameDLL.h"

//opengl Include
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Debug.h"

void windowSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

int GameLoop()
{
	int screenWidth = 1200;
	int screenHeight = 800;


	if (!glfwInit())
	{
		Logger::Error("glfwInit failed\n");
		return 0;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, TemplateManager::LoadGeneral("general").c_str(), nullptr, nullptr);
	if (window == nullptr)
	{
		Logger::Error("glfwCreateWindow failed\n");
		return 0;
	}
	glfwMakeContextCurrent(window);


	if (gladLoaderLoadGL() == 0)
	{
		Logger::Error("gladLoaderLoadGL failed\n");
		return 0;
	}

	Engine engine;
	Game game = Game(&engine, window);
	game.Init(window, { (float)screenWidth, (float)screenHeight });
	game.InGame = true;

	//glfwSetWindowSizeCallback(window, windowSizeCallback);
	game.CreateGame();


	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.WindowBorderSize = 0;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");


	while (glfwWindowShouldClose(window) == false)
	{
		glfwPollEvents();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		glViewport(0, 0, w, h);

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

		game.gameUpdate(window);
		game.mousePicking.Update(&engine, window, { (float)w, (float)h }, engine.scene.camera, game.m_charactersTemplate, game.m_buildingsTemplate, engine.scene.characterEntities, engine.scene.buildingEntities, game.engine->scene.map, game.gameInfo.selectedEntity, game.gameInfo.selectedCreatableBuilding);
		game.miniMap.Update(engine.scene.map, engine.scene.characterEntities, engine.scene.buildingEntities, &engine, window, { (float)w,(float)h });
		engine.Display(game.m_charactersTemplate, game.m_buildingsTemplate, game.InGame, { static_cast<float>(w), static_cast<float>(h) }, game.economySystem);

		// Draw gizmo
		DrawGizmo({ (float)w, (float)h }, game.mousePicking.lastClickedPos, engine.scene.camera.projMat * engine.scene.camera.viewMat);
		// Draw circle
		DrawRange({ (float)w, (float)h }, game.mousePicking.lastClickedPos, engine.scene.camera.projMat * engine.scene.camera.viewMat, 0.2f);
		//DrawQuad({ (float)w, (float)h }, game.mousePicking.lastClickedPos, engine.scene.camera.projMat * engine.scene.camera.viewMat, { 1.f, 1.f });


		glUseProgram(engine.renderer.m_gameProgram);
		glBindTexture(GL_TEXTURE_2D, engine.scene.GetCBO());
		//glBindTexture(GL_TEXTURE_2D, 3);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);

		engine.renderer.RenderQuad();
		glUseProgram(0);

		game.uiUpdate({ (float)w, (float)h });

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGui::EndFrame();

		glfwSwapBuffers(window);
	}

	return 1;
}


int main()
{
	int i = GameLoop();

	return i;
}
