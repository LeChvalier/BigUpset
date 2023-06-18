#pragma once

#include <string>
#include <filesystem>


class ContentBrowserPanel
{
public: 
	ContentBrowserPanel();
	void Init();

	void ImguiRender();
	
	float thumbnailSize = 70.f; //ImGui content browser button size
private:
	std::filesystem::path m_CurrentDirectory;

	unsigned int m_folderBrowserTexture = 0;
	unsigned int m_pngBrowserTexture = 0;
	unsigned int m_jpgBrowserTexture = 0;
	unsigned int m_objBrowserTexture = 0;
	unsigned int m_fbxBrowserTexture = 0;
	unsigned int m_mp3BrowserTexture = 0;
	unsigned int m_scriptBrowserTexture = 0;
	unsigned int m_error = 0;
};