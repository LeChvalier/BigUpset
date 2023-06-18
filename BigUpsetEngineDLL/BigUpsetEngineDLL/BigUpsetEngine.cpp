#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "BigUpsetEngine.h"

#include "Logger.h"


void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    std::string str = " OpenGL error: %s\n";
    str += message;

    Logger::Error(str);
}

Engine::Engine()
    : resourceManager(&renderer)
    , scene(&renderer, &resourceManager)
    , preview(&renderer, &resourceManager)
{

}



Engine::~Engine() {}

void Engine::Display(std::list<UnitDefinition>& charaters, std::list<UnitDefinition>& buildings, const bool& inGame, const float2& windowSize, EconomySystem& economySystem)
{
    scene.UpdateAndRender(charaters, buildings, inGame, windowSize, economySystem);
    preview.UpdateAndRender(windowSize);
}