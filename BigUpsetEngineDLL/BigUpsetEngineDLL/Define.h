#pragma once

#include <string>

#pragma region FILE_PATH 

// General
#define ASSETS "../assets/"
#define DEFAULT_STRING "Default string"

// Editor
#define FOLDER			"../assets/DefaultFiles/Folder.png"
#define PNG				"../assets/DefaultFiles/PNG.png"
#define JPG				"../assets/DefaultFiles/JPG.png"
#define OBJ				"../assets/DefaultFiles/OBJ.png"
#define FBX				"../assets/DefaultFiles/FBX.png"
#define MP3				"../assets/DefaultFiles/MP3.png"
#define SCRIPT			"../assets/DefaultFiles/C++.png"

// Models
#define DEFAULT_MESH	"../assets/DefaultFiles/Default.obj"
#define QUAD			"../assets/DefaultFiles/quad.obj"

// Textures
#define DEFAULT_TEXT	"../assets/DefaultFiles/Default.png"

// Sounds
#define DEFAULT_SOUND   "DefaultFiles/DefaultSound.ogg"

// Templates
#define CHARACTER		"../templates/characters/"
#define BUILDING		"../templates/buildings/"
#define DECOR			"../templates/decors/"
#define CAMERA			"../templates/camera/"
#define SOUND_INFO		"../templates/soundInfo/"
#define WAVE_INFO		"../templates/waveInfo/"
#define MAP				"../templates/map/"
#define ECONOMY			"../templates/economy/"
#define GENERAL			"../templates/general/"
#define PARTICLE		"../templates/particles/"
#define EXTENTION		".bup"

#pragma endregion // FILE_PATH


#pragma region COLOR

// ImU32
#define RED IM_COL32(0xFF, 0x00, 0x00, 0xFF)
#define GREEN IM_COL32(0x00, 0xFF, 0x00, 0xFF)
#define BLUE IM_COL32(0x00, 0x00, 0xFF, 0xFF)

#pragma endregion // COLOR

#pragma region OTHER

// Camera
#define FOY 1.5
#define FAR 200
#define NEAR 0.001
#define MIN_HEIGHT 2.f
#define MAX_HEIGHT 7.f

#pragma endregion // OTHER
