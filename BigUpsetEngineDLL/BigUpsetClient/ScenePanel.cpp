#include <imgui.h>
#include <ImGuizmo.h>

#include "Preview.h"
#include "ScenePanel.h"


void ScenePanel::ImguiRender(Preview* preview)
{
    if (ImGui::Begin("Scene"))
    {          
        // Render the scene CBO
        //ImGui::GetCursorPos() x, y du viewport
        ImVec2 v = ImGui::GetCursorPos();
        ImVec2 w = ImGui::GetWindowPos();
        ImVec2 m = ImGui::GetMousePos();

        ImGui::Image((void*)(intptr_t)preview->GetCBO(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

        // Gizmos
        /*
        if (!scene->dynamicObjects.empty() && gizmoType != -1)
        {
            ObjectPointer& selectedEntity = scene->dynamicObjects[0];
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
            
            mat4& cameraProj = scene->camera.projMat;
            mat4& cameraView = scene->camera.viewMat; 
            
            auto& tc = selectedEntity.transform;
            mat4 transform = tc;
            
            ImGuizmo::Manipulate(cameraView.e, cameraProj.e, (ImGuizmo::OPERATION)gizmoType, ImGuizmo::LOCAL, selectedEntity.transform.e);
            
            if (ImGuizmo::IsUsing())
            {
                ImGuizmo::DecomposeMatrixToComponents(selectedEntity.transform.e, selectedEntity.position.e, selectedEntity.rotation.e, selectedEntity.scale.e);
            }
        }*/      
    }
    ImGui::End();
}
