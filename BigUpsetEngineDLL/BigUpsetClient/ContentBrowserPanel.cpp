#include <stb_image.h>
#include <glad/gl.h>
#include <iostream>
#include <imgui.h>

#include "ContentBrowserPanel.h"
#include "TextureLoader.h"
#include "Logger.h"
#include "Renderer.h"

#include "Define.h"

ContentBrowserPanel::ContentBrowserPanel()
	: m_CurrentDirectory(ASSETS)
{
}

GLuint GenTexture(const char* path)
{
	GLuint result;
	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_2D, result);
	UploadTexture(path, 0);
	glGenerateTextureMipmap(result);
	
	return result; // TODO: Return 0 on error
}

void ContentBrowserPanel::Init()
{
	//Load texture for all extention type (.py, .png, .fbx.....)
	m_folderBrowserTexture = GenTexture(FOLDER);
	m_pngBrowserTexture = GenTexture(PNG);
	m_jpgBrowserTexture = GenTexture(JPG);
	m_objBrowserTexture = GenTexture(OBJ);
	m_fbxBrowserTexture = GenTexture(FBX);
	m_mp3BrowserTexture = GenTexture(MP3);
	m_scriptBrowserTexture = GenTexture(SCRIPT);
	m_error = GenTexture(DEFAULT_TEXT);
	// TODO: Handle errors
	// TODO: Make array of icons?
}

void ContentBrowserPanel::ImguiRender()
{
	ImGui::Begin("Content Browser");

	//Back to previous folder
	if (m_CurrentDirectory != "../assets")
	{
		if (ImGui::Button("Back"))
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
	}
	
	//Set a number of columns according to the size
	float cellSize = thumbnailSize;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);

	if (columnCount < 1) columnCount = 1;
	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		//Get the actual path
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, ASSETS);
		std::string filenameString = relativePath.filename().string();
		ImGui::PushID(filenameString.c_str());

		//Set up the texture according to the extension
		GLuint icon;

		if (directoryEntry.is_directory())
			icon = m_folderBrowserTexture;
		else if (directoryEntry.path().extension() == ".png" || directoryEntry.path().extension() == ".tga")
			icon = m_pngBrowserTexture;
		else if (directoryEntry.path().extension() == ".jpg")
			icon = m_jpgBrowserTexture;
		else if (directoryEntry.path().extension() == ".obj")
			icon = m_objBrowserTexture;
		else if (directoryEntry.path().extension() == ".fbx")
			icon = m_fbxBrowserTexture;
		else if (directoryEntry.path().extension() == ".ogg" || directoryEntry.path().extension() == ".mp3" || directoryEntry.path().extension() == ".wav")
			icon = m_mp3BrowserTexture;
		else if (directoryEntry.path().extension() == ".py")
			icon = m_scriptBrowserTexture;
		else
			icon = m_error;
			
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // make button transparent
		ImGui::ImageButton((ImTextureID)(size_t)icon, { thumbnailSize, thumbnailSize });
		ImGui::PopStyleColor();

		//Change relativePath to a string for starting drag and drop
		std::string itemPath = relativePath.string().c_str();
	
		//Starting drag and drop for and texture and model
		std::string ext = itemPath.substr(itemPath.length() - 4);
		if (ext == ".png" || ext == ".jpg" || ext == ".tga")
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE", itemPath.c_str(), (itemPath.size()+1) * sizeof(char));
				ImGui::EndDragDropSource();
			}
		}
		if (ext == ".obj" || ext == ".fbx")
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM_MESH", itemPath.c_str(), (itemPath.size()+1) * sizeof(char));
				ImGui::EndDragDropSource();
			}
		}
		if (ext == ".ogg")
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM_SOUND", itemPath.c_str(), (itemPath.size() + 1) * sizeof(char));
				ImGui::EndDragDropSource();
			}
		}

		//Open Visual code with the good script when doubleClicked on .py
		if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
				m_CurrentDirectory /= path.filename(); 
			else if (directoryEntry.path().extension() == ".py")
			{
				Logger::Info("Open " + itemPath);
				std::string teststr = "code -g " + itemPath;
				std::system(teststr.c_str());
			}
		}

		ImGui::Text(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();

	}
	ImGui::Columns(1);

	ImGui::End();
}
