#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Editor.h"
#include "Maths.h"
#include "Logger.h"


Editor::Editor(Engine* _engine, Game* _game, GLFWwindow* _window)
{
	engine = _engine;
    game = _game;

    engine->scene.camera = { 0.f, 0.3f, 1.5f };

    //Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;          // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    dockspace_flags = ImGuiDockNodeFlags_None;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    Style();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    game->engine->scene.camera = TemplateManager::LoadCamera("Camera");
}

void Editor::Init()
{
    m_ContentBrowserPanel.Init();

    m_LevelPanel.Init(game);
}

void Editor::Style()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.TabRounding = 3;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 2;
    style.WindowMenuButtonPosition = 0;
    style.WindowBorderSize = 0;
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(23.f / 255.f, 23.f / 255.f, 23.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(150.f / 255.f, 150.f / 255.f, 150.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(150.f / 255.f, 150.f / 255.f, 150.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_Tab] = ImVec4(45.f / 255.f, 45.f / 255.f, 45.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(120.f / 255.f, 120.f / 255.f, 120.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(70.f / 255.f, 70.f / 255.f, 70.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(45.f / 255.f, 45.f / 255.f, 45.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(70.f / 255.f, 70.f / 255.f, 70.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_DockingPreview] = ImVec4(200.f / 255.f, 200.f / 255.f, 200.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_Button] = ImVec4(96.f / 255.f, 96.f / 255.f, 96.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(116.f / 255.f, 116.f / 255.f, 116.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(139.f / 255.f, 139.f / 255.f, 139.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(169.f / 255.f, 169.f / 255.f, 169.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_Header] = ImVec4(103.f / 255.f, 103.f / 255.f, 103.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(116.f / 255.f, 116.f / 255.f, 116.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(190.f / 255.f, 190.f / 255.f, 190.f / 255.f, 1.00f);
    
    style.Colors[ImGuiCol_CheckMark] = ImVec4(220.f / 255.f, 220.f / 255.f, 220.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_SliderGrab] = ImVec4(170.f / 255.f, 170.f / 255.f, 170.f / 255.f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(200.f / 255.f, 200.f / 255.f, 200.f / 255.f, 1.00f);

    style.Colors[ImGuiCol_NavHighlight] = ImVec4(220.f / 255.f, 220.f / 255.f, 220.f / 255.f, 1.00f);
}

Editor::~Editor()
{
}

void Editor::Update(const float2& windowSize)
{
    UpdateImgui(windowSize);
}

void Editor::UpdateImgui(const float2& windowSize)
{
    ImGuiIO& io = ImGui::GetIO();

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    //Setup Window, Docking and Padding
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
    }

    //Update all widget
    WidgetMenuBar();
    WidgetResources();
    WidgetEntities();
    WidgetConsole();
    WidgetScene();
    WidgetGame();
    WidgetParticlesSystem();
    WidgetComponents();
    WidgetLevel();

    if (game->InGame)
        game->uiUpdate(windowSize);

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}


void Editor::WidgetMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::Button("Save Project"))
            {
                for (UnitDefinition& charaDef : game->m_charactersTemplate)
                {
                    TemplateManager::WriteCharacter(charaDef, charaDef.m_name);
                }

                for (UnitDefinition& buildDef : game->m_buildingsTemplate)
                {
                    TemplateManager::WriteBuilding(buildDef, buildDef.m_name);
                }

                for (DecorDefinition& decorDef : game->m_decorsTemplate)
                {
                    TemplateManager::WriteDecor(decorDef, decorDef.m_name);
                }

                for (ParticlesSystemDefinition& particleDef : game->m_particlesTemplate)
                {
                    TemplateManager::WriteParticle(particleDef, particleDef.m_name);
                }

                TemplateManager::WriteCamera(game->engine->scene.camera, "Camera");
                TemplateManager::WriteEconomySystem(game->economySystem, "economy");
                TemplateManager::WriteGeneral(game->name, "general");
                TemplateManager::WriteMap(game->engine->scene.map, "map");
                TemplateManager::WriteSoundInfo(game->soundInfo, "soundInfo");
                TemplateManager::WriteWaveInfo(game->waveInfo, "waveInfo");

                Logger::Info("All project saved");
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            const char* itemsTheme[] = { "BigUpset Theme", "Black Theme", "White Theme" , "Basic Theme"};
            static int currentTheme = 0;
            if (ImGui::Combo("Theme", &currentTheme, itemsTheme, IM_ARRAYSIZE(itemsTheme), IM_ARRAYSIZE(itemsTheme)))
            {
                switch (currentTheme)
                {
                case 1:
                    ImGui::StyleColorsDark();
                    break;
                case 2:
                    ImGui::StyleColorsLight();
                    break;
                case 3:
                    ImGui::StyleColorsClassic();
                    break;
                default:
                    Style();
                    break;
                }
            }

            const char* itemsLogPriority[] = { "InfoPriority", "WarnPriority", "ErrorPriority" , "CriticalPriority"};
            static int currentLogPriority = 0;
            if (ImGui::Combo("Log Priority", &currentLogPriority, itemsLogPriority, IM_ARRAYSIZE(itemsLogPriority), IM_ARRAYSIZE(itemsLogPriority)))
            {
                switch (currentLogPriority)
                {
                case 1:
                    Logger::SetPriority(LogPriority::WarnPriority);
                    break;
                case 2:
                    Logger::SetPriority(LogPriority::ErrorPriority);
                    break;
                case 3:
                    Logger::SetPriority(LogPriority::CriticalPriority);
                    break;
                default:
                    Logger::SetPriority(LogPriority::InfoPriority);
                    break;
                }
            }

            if (ImGui::Button("Test Priority"))
            {
                Logger::Info("Test Priority");
                Logger::Warn("Test Priority");
                Logger::Error("Test Priority");
                Logger::Critical("Test Priority");
            }

            ImGui::SliderFloat("Browser Content size", &m_ContentBrowserPanel.thumbnailSize, 55, 170);
            ImGui::SliderFloat("Entity size", &m_EntityPanel.thumbnailSize, 80, 250);
            ImGui::SliderFloat("Particle size", &m_ParticlePanel.thumbnailSize, 80, 250);
            ImGui::SliderFloat("General settings size", &m_LevelPanel.thumbnailSize, 80, 250);

            ImGui::EndMenu();
        }
        /*if (ImGui::BeginMenu("Guizmo"))
        {
            const char* itemsGuizmo[] = { "None", "Translation", "Rotation", "Scale" , "TRS"};
            static int currentGuizmo = 0;
            if (ImGui::Combo("Guizmo type", &currentGuizmo, itemsGuizmo, IM_ARRAYSIZE(itemsGuizmo), IM_ARRAYSIZE(itemsGuizmo)))
            {
                switch (currentGuizmo)
                {
                case 1:
                    m_ScenePanel.gizmoType = ImGuizmo::OPERATION::TRANSLATE;
                    break;
                case 2:
                    m_ScenePanel.gizmoType = ImGuizmo::OPERATION::ROTATE;
                    break;
                case 3:
                    m_ScenePanel.gizmoType = ImGuizmo::OPERATION::SCALE;
                    break;
                case 4:
                    m_ScenePanel.gizmoType = ImGuizmo::OPERATION::UNIVERSAL;
                    break;
                default:
                    m_ScenePanel.gizmoType = -1;
                    break;
                }
            }
            ImGui::EndMenu();
        }
        */
        ImGui::EndMenuBar();
    }
}

void Editor::WidgetScene()
{
    m_ScenePanel.ImguiRender(&engine->preview);
}

void Editor::WidgetGame()
{
    m_GamePanel.ImguiRender(game, gameScreenSize, gameScreenPos);
}

void Editor::WidgetResources()
{
    m_ContentBrowserPanel.ImguiRender();
}

void Editor::WidgetEntities()
{
    m_EntityPanel.ImguiRender(game, engine, selected);
}

void Editor::WidgetLevel()
{
    m_LevelPanel.ImguiRender(engine, game, selected);
}


void Editor::WidgetParticlesSystem()
{
    m_ParticlePanel.ImguiRender(engine, game, selected);
}

void Editor::WidgetConsole()
{
    m_LoggerPanel.ImguiRender();
}

void Editor::WidgetComponents()
{
    if (ImGui::Begin("Components"))
    {

    }
    ImGui::End();
}
