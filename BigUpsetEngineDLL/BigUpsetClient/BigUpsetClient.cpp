#include "BigUpsetEngine.h"
#include "GameDLL.h"

#include "Logger.h"

#include "Maths.h"
#include "Editor.h"
#include "Objects.h"

//std Include
#include <iostream>

//imgui Include
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//opengl Include
#include <glad/gl.h>
#include <GLFW/glfw3.h>


int main()
{
    // Initial Screen size
    int screenWidth = 1200;
    int screenHeight = 800;

    if (!glfwInit())
    {
        Logger::Error("glfwInit failed\n");
        return 1;
    }

    // Create OPENGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "BigUpset", nullptr, nullptr);
    if (window == nullptr)
    {
        Logger::Error("glfwCreateWindow failed\n");
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (gladLoaderLoadGL() == 0)
    {
        Logger::Error("gladLoaderLoadGL failed\n");
        return 1;
    }

    Logger::SetPriority(LogPriority::InfoPriority);
    Logger::EnableFileOuput("logs/logs.txt");

    Engine engine;

    Game game = Game(&engine, window );
    game.Init(window, { (float)screenWidth, (float)screenHeight });

    Editor editor = Editor(&engine, &game, window);

    editor.Init();

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (game.InGame)
        {
            game.gameUpdate(window);
            game.mousePicking.Update(&engine, window, editor.gameScreenSize, engine.scene.camera, game.m_charactersTemplate, game.m_buildingsTemplate, engine.scene.characterEntities, engine.scene.buildingEntities, game.engine->scene.map, game.gameInfo.selectedEntity, game.gameInfo.selectedCreatableBuilding, 1.f * editor.gameScreenPos);
            game.miniMap.Update(engine.scene.map, engine.scene.characterEntities, engine.scene.buildingEntities, &engine, window, { (float)screenWidth,(float)screenHeight });
            //std::cout << editor.gameScreenSize.x << " " << editor.gameScreenSize.y << std::endl;
            //std::cout << editor.gameScreenPos.x << " " << editor.gameScreenPos.y << std::endl;
        }

        engine.Display(game.m_charactersTemplate, game.m_buildingsTemplate, game.InGame, { (float)screenWidth,(float)screenHeight }, game.economySystem);
        editor.Update({ (float)screenWidth,(float)screenHeight });
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}