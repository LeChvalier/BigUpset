#include "LoggerPanel.h"
#include "Logger.h"


void LoggerPanel::ImguiRender()
{
    if (ImGui::Begin("Console"))
    {
        CheckLogList();

        // Debug menu
        if (ImGui::BeginPopup("Debug"))
        {
            if (ImGui::SmallButton("Add Debug Error")) { AddLog("[ERROR] THIS IS A DEBUG ERROR"); }
            if (ImGui::SmallButton("Add Debug Warn")) { AddLog("[WARNING] THIS IS A DEBUG WARNING"); }
            ImGui::EndPopup();
        }
        ImGui::SameLine();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            if (ImGui::SmallButton("Clear Logs")) { ClearLog(); }
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::EndPopup();
        }
        ImGui::SameLine();

        // Options, Filter
        if (ImGui::Button("Debug"))
            ImGui::OpenPopup("Debug");
        ImGui::SameLine();
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        filter.Draw("Filter (\"error\")(\"warn\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }
        for (int i = 0; i < items.Size; i++)
        {
            const char* item = items[i];
            if (!filter.PassFilter(item))
                continue;

            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[ERROR]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strstr(item, "[WARNING]")) { color = ImVec4(1.0f, 0.85f, 0.2f, 1.00f); has_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }

        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;

        ImGui::EndChild();
    }
    ImGui::End();
}

void LoggerPanel::CheckLogList()
{
    if (!Logger::logList.empty())
    {
        for (int i = 0; i < Logger::logList.size(); i++)
        {
            AddLog(Logger::logList[i].c_str());
        }

        Logger::logList.clear();
    }
}

void LoggerPanel::ClearLog()
{
    for (int i = 0; i < items.Size; i++)
        free(items[i]);
    items.clear();
}

void LoggerPanel::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    items.push_back(strdup(buf));
}