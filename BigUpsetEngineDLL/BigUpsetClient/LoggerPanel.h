#pragma once

#include <imgui.h>


class LoggerPanel
{
public:
	//Draw Console in the Editor
	void ImguiRender();

	//Check & draw new log line
	void CheckLogList();
	//Clear log console in editor
	void ClearLog();
	//Add new log in console
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);

	char                  inputBuf[256];
	ImVector<char*>       items;
	ImVector<const char*> commands;
	ImVector<char*>       history;
	int                   historyPos;                // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter       filter;
	bool                  autoScroll = true;
	bool                  scrollToBottom;
};