#pragma warning(disable : 4996)

#include <tinyxml2.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>

#include "TemplateManager.h"
#include "SoundInfo.h"
#include "WaveInfo.h"
#include "Logger.h"
#include "Entity.h"
#include "Camera.h"
#include "Map.h"
#include "Economy.h"

#include "Define.h"

using namespace tinyxml2;

int TemplateManager::GetIntValue(void* element, const char* name, int defaultValue)
{
	XMLElement* pElement = static_cast<XMLElement*>(element)->FirstChildElement(name);
	if (NULL != pElement)
	{
		int value = (int)strtof(pElement->GetText(), nullptr);
		return value;
	}
	return defaultValue;
}

float TemplateManager::GetFloatValue(void* element, const char* name, float defaultValue)
{
	XMLElement* pElement = static_cast<XMLElement*>(element)->FirstChildElement(name);
	if (NULL != pElement)
	{
		float value = strtof(pElement->GetText(), nullptr);
		return value;
	}
	return defaultValue;
}

const char* TemplateManager::GetCharValue(void* element, const char* name, const char* defaultValue)
{
	XMLElement* pElement = static_cast<XMLElement*>(element)->FirstChildElement(name);
	if (NULL != pElement)
	{
		const char* value = pElement->GetText();
		return value;
	}
	return defaultValue;
}

void TemplateManager::SetIntValue(void* element, const char* name, int value, tinyxml2::XMLDocument* xmlDoc)
{
	XMLElement* pElement = xmlDoc->NewElement(name);
	pElement->SetText(value);
	static_cast<XMLElement*>(element)->InsertFirstChild(pElement);
}

void TemplateManager::SetFloatValue(void* element, const char* name, float value, tinyxml2::XMLDocument* xmlDoc)
{
	XMLElement* pElement = xmlDoc->NewElement(name);
	pElement->SetText(value);
	static_cast<XMLElement*>(element)->InsertFirstChild(pElement);
}

void TemplateManager::SetCharValue(void* element, const char* name, const char* value, tinyxml2::XMLDocument* xmlDoc)
{
	XMLElement* pElement = xmlDoc->NewElement(name);
	pElement->SetText(value);
	static_cast<XMLElement*>(element)->InsertEndChild(pElement);
}


UnitDefinition TemplateManager::LoadCharacter(const std::string& name)
{
	//std::string filePath = characterPath + name + extention;
	std::string filePath = CHARACTER + name + EXTENTION;
	UnitDefinition character;
	character.m_type = UT_CHARACTER;

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + name;
		Logger::Error(str);
		return character;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pCharacters = pRootElement->FirstChildElement("Characters");
		if (NULL != pCharacters)
		{
			character.m_maxlife = GetIntValue(pCharacters, "Life", 0);

			character.m_name = GetCharValue(pCharacters, "Name", "default_name");

			character.m_speed = GetIntValue(pCharacters, "Speed", 0);

			character.pathMesh = GetCharValue(pCharacters, "PathMesh", DEFAULT_MESH);

			character.pathTexture = GetCharValue(pCharacters, "PathTexture", DEFAULT_TEXT);

			character.m_attackRange = GetIntValue(pCharacters, "AttackRange", 0);
			character.m_attackSpeed = GetIntValue(pCharacters, "AttackSpeed", 0);
			character.m_canAttack = GetIntValue(pCharacters, "CanAttack", 0);
			character.m_damage = GetIntValue(pCharacters, "Damage", 0);

			character.scriptType = (ScriptType)GetIntValue(pCharacters, "ScriptTypeIndex", 0);
			character.m_side = (AISideType)GetIntValue(pCharacters, "AISideTypeIndex", 0);

			character.m_shotSound = GetCharValue(pCharacters, "ShotSound", DEFAULT_SOUND);
			character.m_dieSound = GetCharValue(pCharacters, "DieSound", DEFAULT_SOUND);

			character.m_price = GetIntValue(pCharacters, "Price", 0);
			character.m_reward = GetIntValue(pCharacters, "Reward", 0);

			character.m_IdAttackParticle = GetIntValue(pCharacters, "AttackParticle", 0);
			character.m_IdDieParticle = GetIntValue(pCharacters, "DieParticle", 0);
		}
	}
	return character;
}

void TemplateManager::WriteCharacter(const UnitDefinition& _character, const std::string& name)
{
	std::string filePath = CHARACTER + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* characters = xmlDoc.NewElement("Characters");
	characters->SetText("\n");
	pRoot->InsertFirstChild(characters);

	SetIntValue(characters, "Life", _character.m_maxlife, &xmlDoc);

	SetCharValue(characters, "Name", _character.m_name.c_str(), &xmlDoc);

	SetIntValue(characters, "Speed", _character.m_speed, &xmlDoc);

	SetCharValue(characters, "PathMesh", _character.pathMesh.c_str(), &xmlDoc);

	SetCharValue(characters, "PathTexture", _character.pathTexture.c_str(), &xmlDoc);

	SetIntValue(characters, "AttackRange", _character.m_attackRange, &xmlDoc);
	SetIntValue(characters, "AttackSpeed", _character.m_attackSpeed, &xmlDoc);
	SetIntValue(characters, "CanAttack", _character.m_canAttack, &xmlDoc);
	SetIntValue(characters, "Damage", _character.m_damage, &xmlDoc);

	SetIntValue(characters, "ScriptTypeIndex", _character.scriptType, &xmlDoc);
	SetIntValue(characters, "AISideTypeIndex", _character.m_side, &xmlDoc);

	SetCharValue(characters, "ShotSound", _character.m_shotSound.c_str(), &xmlDoc);
	SetCharValue(characters, "DieSound", _character.m_dieSound.c_str(), &xmlDoc);

	SetIntValue(characters, "Price", _character.m_price, &xmlDoc);
	SetIntValue(characters, "Reward", _character.m_reward, &xmlDoc);

	SetIntValue(characters, "AttackParticle", _character.m_IdAttackParticle, &xmlDoc);
	SetIntValue(characters, "DieParticle", _character.m_IdDieParticle, &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());
	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

UnitDefinition TemplateManager::LoadBuilding(const std::string& name)
{
	std::string filePath = BUILDING + name + EXTENTION;

	UnitDefinition building = {};
	building.m_type = UT_BUILDING;

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();
	
	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return building;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pBuilding = pRootElement->FirstChildElement("Buildings");
		if (NULL != pBuilding)
		{
			//Life
			building.m_maxlife = GetIntValue(pBuilding, "Life", 0);

			//	Name
			building.m_name = GetCharValue(pBuilding, "Name", "default_name");

			//	Speed
			building.m_cooldownCreateCharacter = GetIntValue(pBuilding, "CooldownCreateCharacter", 0);

			//	PathMesh
			building.pathMesh = GetCharValue(pBuilding, "PathMesh", DEFAULT_MESH);

			//	PathTexture
			building.pathTexture = GetCharValue(pBuilding, "PathTexture", DEFAULT_TEXT);

			building.m_attackRange = GetIntValue(pBuilding, "AttackRange", 0);
			building.m_attackSpeed = GetIntValue(pBuilding, "AttackSpeed", 0);
			building.m_canAttack = GetIntValue(pBuilding, "CanAttack", 0);
			building.m_canCreateCharacter = GetIntValue(pBuilding, "CanCreateCharacter", 0);
			building.m_mainBuilding = GetIntValue(pBuilding, "MainBuilding", 0);
			building.m_damage = GetIntValue(pBuilding, "Damage", 0);

			building.scriptType = (ScriptType)GetIntValue(pBuilding, "ScriptTypeIndex", 0);
			building.m_side = (AISideType)GetIntValue(pBuilding, "AISideTypeIndex", 0);
			
			building.m_IdUnitDefCreateable = (ScriptType)GetIntValue(pBuilding, "ScriptTypeIndexCreateable", 0);

			building.m_shotSound = GetCharValue(pBuilding, "ShotSound", DEFAULT_SOUND);
			building.m_dieSound = GetCharValue(pBuilding, "DieSound", DEFAULT_SOUND);

			building.m_price = GetIntValue(pBuilding, "Price", 0);
			building.m_reward = GetIntValue(pBuilding, "Reward", 0);

			building.m_IdAttackParticle = GetIntValue(pBuilding, "AttackParticle", 0);
			building.m_IdDieParticle = GetIntValue(pBuilding, "DieParticle", 0);
		}
	}
	return building;
}

void TemplateManager::WriteBuilding(const UnitDefinition& _building, const std::string& name)
{
	std::string filePath = BUILDING + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* buildings = xmlDoc.NewElement("Buildings");
	buildings->SetText("\n");
	pRoot->InsertFirstChild(buildings);

	SetIntValue(buildings, "Life", _building.m_maxlife, &xmlDoc);

	SetCharValue(buildings, "Name", _building.m_name.c_str(), &xmlDoc);

	SetIntValue(buildings, "CooldownCreateCharacter", _building.m_cooldownCreateCharacter, &xmlDoc);

	SetCharValue(buildings, "PathMesh", _building.pathMesh.c_str(), &xmlDoc);

	SetCharValue(buildings, "PathTexture", _building.pathTexture.c_str(), &xmlDoc);

	SetIntValue(buildings, "AttackRange", _building.m_attackRange, &xmlDoc);
	SetIntValue(buildings, "AttackSpeed", _building.m_attackSpeed, &xmlDoc);
	SetIntValue(buildings, "CanAttack", _building.m_canAttack, &xmlDoc);
	SetIntValue(buildings, "CanCreateCharacter", _building.m_canCreateCharacter, &xmlDoc);
	SetIntValue(buildings, "MainBuilding", _building.m_mainBuilding, &xmlDoc);

	SetIntValue(buildings, "Damage", _building.m_damage, &xmlDoc);

	SetIntValue(buildings, "ScriptTypeIndex", _building.scriptType, &xmlDoc);
	SetIntValue(buildings, "AISideTypeIndex", _building.m_side, &xmlDoc);
	SetIntValue(buildings, "ScriptTypeIndexCreateable", _building.m_IdUnitDefCreateable, &xmlDoc);

	SetCharValue(buildings, "ShotSound", _building.m_shotSound.c_str(), &xmlDoc);
	SetCharValue(buildings, "DieSound", _building.m_dieSound.c_str(), &xmlDoc);

	SetIntValue(buildings, "Price", _building.m_price, &xmlDoc);
	SetIntValue(buildings, "Reward", _building.m_reward, &xmlDoc);

	SetIntValue(buildings, "AttackParticle", _building.m_IdAttackParticle, &xmlDoc);
	SetIntValue(buildings, "DieParticle", _building.m_IdDieParticle, &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

Camera TemplateManager::LoadCamera(const std::string& name)
{
	std::string filePath = CAMERA + name + EXTENTION;

	Camera camera = {};

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return camera;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pCam = pRootElement->FirstChildElement("Camera");
		if (NULL != pCam)
		{
			camera.far   = GetFloatValue(pCam, "Far", 0);
			camera.near  = GetFloatValue(pCam, "Near", 0);
			camera.pitch = GetFloatValue(pCam, "Pitch", 0);
			camera.roll  = GetFloatValue(pCam, "Roll", 0);
			camera.yaw   = GetFloatValue(pCam, "Yaw", 0);

			camera.position.x = GetFloatValue(pCam, "PosX", 0);
			camera.position.y = GetFloatValue(pCam, "PosY", 0);
			camera.position.z = GetFloatValue(pCam, "PosZ", 0);

			camera.normalSpeed = GetFloatValue(pCam, "NormalSpeed", 0);
			camera.fastSpeed = GetFloatValue(pCam, "FastSpeed", 0);
		}
	}
	return camera;
}

void TemplateManager::WriteCamera(Camera camera, const std::string& name)
{
	std::string filePath = CAMERA + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* cam = xmlDoc.NewElement("Camera");
	cam->SetText("\n");
	pRoot->InsertFirstChild(cam);

	SetFloatValue(cam, "PosX", camera.position.x, &xmlDoc);
	SetFloatValue(cam, "PosY", camera.position.y, &xmlDoc);
	SetFloatValue(cam, "PosZ", camera.position.z, &xmlDoc);

	SetFloatValue(cam, "Far", camera.far, &xmlDoc);
	SetFloatValue(cam, "Near", camera.near, &xmlDoc);
	SetFloatValue(cam, "Pitch", camera.pitch, &xmlDoc);
	SetFloatValue(cam, "Roll", camera.roll, &xmlDoc);
	SetFloatValue(cam, "Yaw", camera.yaw, &xmlDoc);

	SetFloatValue(cam, "NormalSpeed", camera.normalSpeed, &xmlDoc);
	SetFloatValue(cam, "FastSpeed", camera.fastSpeed, &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

std::string TemplateManager::LoadGeneral(const std::string& name)
{
	std::string filePath = GENERAL + name + EXTENTION;

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return DEFAULT_STRING;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pEconomy = pRootElement->FirstChildElement("Camera");
		if (NULL != pEconomy)
			return GetCharValue(pEconomy, "GameName", "Default_name");
	}
	return DEFAULT_STRING;
}

void TemplateManager::WriteGeneral(std::string gameName, const std::string& name)
{
	std::string filePath = GENERAL + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* economy = xmlDoc.NewElement("Camera");
	economy->SetText("\n");
	pRoot->InsertFirstChild(economy);

	SetCharValue(economy, "GameName", gameName.c_str(), &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

EconomySystem TemplateManager::LoadEconomySystem(const std::string& name)
{
	std::string filePath = ECONOMY + name + EXTENTION;

	EconomySystem economySystem = {};

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return economySystem;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pEconomy = pRootElement->FirstChildElement("EconomySystem");
		if (NULL != pEconomy)
		{
			economySystem.maxMoney = GetIntValue(pEconomy, "MaxMoney", 1000);
			economySystem.moneyPerSec = GetIntValue(pEconomy, "MoneyPerSec", 2);
			economySystem.startingMoney = GetIntValue(pEconomy, "StartingMoney", 100);

			economySystem.name = GetCharValue(pEconomy, "Name", "Default_name");
		}
	}
	return economySystem;

}
void TemplateManager::WriteEconomySystem(EconomySystem economySystem, const std::string& name)
{
	std::string filePath = ECONOMY + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* economy = xmlDoc.NewElement("EconomySystem");
	economy->SetText("\n");
	pRoot->InsertFirstChild(economy);

	SetFloatValue(economy, "MaxMoney", economySystem.maxMoney, &xmlDoc);
	SetFloatValue(economy, "MoneyPerSec", economySystem.moneyPerSec, &xmlDoc);
	SetFloatValue(economy, "StartingMoney", economySystem.startingMoney, &xmlDoc);

	SetCharValue(economy, "Name", economySystem.name.c_str(), &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

Map TemplateManager::LoadMap(const std::string& name)
{
	std::string filePath = MAP + name + EXTENTION;

	Map map = { };
	map.model = Model();
	// TODO delete
	map.model.mesh = new Mesh();
	map.model.texture = new Texture();

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return map;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pMap = pRootElement->FirstChildElement("Map");
		if (NULL != pMap)
		{
			// Textures
			map.model.texture->path = GetCharValue(pMap, "TextureMap", DEFAULT_TEXT);
			map.gameplayMap = GetCharValue(pMap, "GameplayMap", DEFAULT_TEXT);
			map.objectMap = GetCharValue(pMap, "ObjectMap", DEFAULT_TEXT);

			// Pos
			map.position.x = GetFloatValue(pMap, "PosX", 0.f);
			map.position.y = GetFloatValue(pMap, "PosY", 0.f);
			map.position.z = GetFloatValue(pMap, "PosZ", 0.f);

			// Rot
			map.rotation.x = GetFloatValue(pMap, "RotX", 0.f);
			map.rotation.y = GetFloatValue(pMap, "RotY", 0.f);
			map.rotation.z = GetFloatValue(pMap, "RotZ", 0.f);

			// Scale
			map.scale.x = GetFloatValue(pMap, "ScaleX", 0.f);
			map.scale.y = GetFloatValue(pMap, "ScaleY", 0.f);
			map.scale.z = GetFloatValue(pMap, "ScaleZ", 0.f);

			// Decors
			map.decor_R_id = GetIntValue(pMap, "Decor_R", 0);
			map.decor_G_id = GetIntValue(pMap, "Decor_G", 0);
			map.decor_B_id = GetIntValue(pMap, "Decor_B", 0);

			// Floor values
			map.floorValue.x = GetFloatValue(pMap, "FloorValueX", 0.f);
			map.floorValue.y = GetFloatValue(pMap, "FloorValueY", 0.f);
			map.floorValue.z = GetFloatValue(pMap, "FloorValueZ", 0.f);

			// Spawn value
			map.spawnValue = GetIntValue(pMap, "SpawnValue", 0);
			map.seed = GetIntValue(pMap, "Seed", 0);
		}
	}
	return map;
}

void TemplateManager::WriteMap(Map& _map, const std::string& name)
{
	std::string filePath = MAP + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* map = xmlDoc.NewElement("Map");
	map->SetText("\n");
	pRoot->InsertFirstChild(map);
	
	// Textures
	SetCharValue(map, "TextureMap", _map.model.texture->path.c_str(), &xmlDoc);
	SetCharValue(map, "GameplayMap", _map.gameplayMap.c_str(), &xmlDoc);
	SetCharValue(map, "ObjectMap", _map.objectMap.c_str(), &xmlDoc);

	// Pos
	SetFloatValue(map, "PosX", _map.position.x, &xmlDoc);
	SetFloatValue(map, "PosY", _map.position.y, &xmlDoc);
	SetFloatValue(map, "PosZ", _map.position.z, &xmlDoc);

	// Rot
	SetFloatValue(map, "RotX", _map.rotation.x, &xmlDoc);
	SetFloatValue(map, "RotY", _map.rotation.y, &xmlDoc);
	SetFloatValue(map, "RotZ", _map.rotation.z, &xmlDoc);

	// Scale
	SetFloatValue(map, "ScaleX", _map.scale.x, &xmlDoc);
	SetFloatValue(map, "ScaleY", _map.scale.y, &xmlDoc);
	SetFloatValue(map, "ScaleZ", _map.scale.z, &xmlDoc);
	
	// Decors
	SetIntValue(map, "Decor_R", _map.decor_R_id, &xmlDoc);
	SetIntValue(map, "Decor_G", _map.decor_G_id, &xmlDoc);
	SetIntValue(map, "Decor_B", _map.decor_B_id, &xmlDoc);

	// Floor values
	SetFloatValue(map, "FloorValueX", _map.floorValue.x, &xmlDoc);
	SetFloatValue(map, "FloorValueY", _map.floorValue.y, &xmlDoc);
	SetFloatValue(map, "FloorValueZ", _map.floorValue.z, &xmlDoc);

	// Spawn value
	SetFloatValue(map, "SpawnValue", _map.spawnValue, &xmlDoc);
	// Seed value
	SetFloatValue(map, "Seed", _map.seed, &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

DecorDefinition TemplateManager::LoadDecor(const std::string& name)
{
	std::string filePath = DECOR + name + EXTENTION;

	DecorDefinition decor = {};

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return decor;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pDecor = pRootElement->FirstChildElement("Decors");
		if (NULL != pDecor)
		{
			//	PathMesh
			decor.pathMesh = GetCharValue(pDecor, "PathMesh", DEFAULT_MESH);

			//	PathTexture
			decor.pathTexture = GetCharValue(pDecor, "PathTexture", DEFAULT_TEXT);
			decor.m_name = GetCharValue(pDecor, "Name", "Default_name");
		}
	}
	return decor;
}

void TemplateManager::WriteDecor(const DecorDefinition& _decor, const std::string& name)
{
	std::string filePath = DECOR + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* decors = xmlDoc.NewElement("Decors");
	decors->SetText("\n");
	pRoot->InsertFirstChild(decors);

	SetCharValue(decors, "PathMesh", _decor.pathMesh.c_str(), &xmlDoc);

	SetCharValue(decors, "PathTexture", _decor.pathTexture.c_str(), &xmlDoc);
	SetCharValue(decors, "Name", _decor.m_name.c_str(), &xmlDoc);


	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

SoundInfo TemplateManager::LoadSoundInfo(const std::string& name)
{
	std::string filePath =  SOUND_INFO + name + EXTENTION;

	SoundInfo soundInfo = {};

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return soundInfo;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pSoundInfo = pRootElement->FirstChildElement("SoundInfo");
		if (NULL != pSoundInfo)
		{
			//	PathMusic
			soundInfo.ambientMusic = GetCharValue(pSoundInfo, "PathMusic", "No music");

			//	Music volume
			soundInfo.volume_AmbientMusic = GetFloatValue(pSoundInfo, "VolumeMusic", 0.5f);
		}
	}
	return soundInfo;
}

void TemplateManager::WriteSoundInfo(const SoundInfo& _soundInfo, const std::string& name)
{
	std::string filePath = SOUND_INFO + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* soundInfo = xmlDoc.NewElement("SoundInfo");
	soundInfo->SetText("\n");
	pRoot->InsertFirstChild(soundInfo);

	SetCharValue(soundInfo, "PathMusic", _soundInfo.ambientMusic.c_str(), &xmlDoc);
	SetFloatValue(soundInfo, "VolumeMusic", _soundInfo.volume_AmbientMusic, &xmlDoc);


	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

WaveInfo TemplateManager::LoadWaveInfo(const std::string& name)
{
	std::string filePath = WAVE_INFO + name + EXTENTION;

	WaveInfo waveInfo = {};

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return waveInfo;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pWaveInfo = pRootElement->FirstChildElement("WaveInfo");
		if (NULL != pWaveInfo)
		{
			//	Number of Wave
			waveInfo.EnemySpawnPointX = GetIntValue(pWaveInfo, "SpawnPointX", 1);
			waveInfo.EnemySpawnPointZ = GetIntValue(pWaveInfo, "SpawnPointZ", 1);
			waveInfo.EnemySpawnRange = GetIntValue(pWaveInfo, "SpawnRange", 1);
			waveInfo.maxWave = GetIntValue(pWaveInfo, "MaxWave", 1);

			for (int i = 0; i < waveInfo.maxWave; i++)
			{
				std::string std = "Wave_" + std::to_string(i + 1) + "_TimerBeforeWave";
				std::string std2 = "Wave_" + std::to_string(i + 1) + "_EnemiesNumber";
				
				Wave wave;
				waveInfo.wave.push_back(wave);

				waveInfo.wave[i].timerBeforeWave = GetIntValue(pWaveInfo, std.c_str(), 1);
				waveInfo.wave[i].EnemiesNumber = GetIntValue(pWaveInfo, std2.c_str(), 1);

				for (int j = 0; j < waveInfo.wave[i].EnemiesNumber; j++)
				{
					EnemiesWave enemiesWave;
					waveInfo.wave[i].enemies.push_back(enemiesWave);

					std::string std3 = "Wave_" + std::to_string(i + 1) + "_Enemy_" + std::to_string(j + 1) + "_Name";
					std::string std4 = "Wave_" + std::to_string(i + 1) + "_Enemy_" + std::to_string(j + 1) + "_Number";

					waveInfo.wave[i].enemies[j].name = GetCharValue(pWaveInfo, std3.c_str(), "Default_Name");
					waveInfo.wave[i].enemies[j].number = GetIntValue(pWaveInfo, std4.c_str(), 1);
				}
			}
		}
	}
	return waveInfo;
}

void TemplateManager::WriteWaveInfo(const WaveInfo& _waveInfo, const std::string& name)
{
	std::string filePath = WAVE_INFO + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* waveInfo = xmlDoc.NewElement("WaveInfo");
	waveInfo->SetText("\n");
	pRoot->InsertFirstChild(waveInfo);

	SetIntValue(waveInfo, "SpawnPointX", _waveInfo.EnemySpawnPointX, &xmlDoc);
	SetIntValue(waveInfo, "SpawnPointZ", _waveInfo.EnemySpawnPointX, &xmlDoc);
	SetIntValue(waveInfo, "SpawnRange", _waveInfo.EnemySpawnRange, &xmlDoc);
	SetIntValue(waveInfo, "MaxWave", _waveInfo.maxWave, &xmlDoc);

	for (int i = 0; i < _waveInfo.wave.size(); i++)
	{
		std::string std = "Wave_" + std::to_string(i + 1) + "_TimerBeforeWave";
		std::string std2 = "Wave_" + std::to_string(i + 1) + "_EnemiesNumber";

		SetIntValue(waveInfo, std.c_str(), _waveInfo.wave[i].timerBeforeWave, &xmlDoc);
		SetIntValue(waveInfo, std2.c_str(), _waveInfo.wave[i].EnemiesNumber, &xmlDoc);

		for (int j = 0; j < _waveInfo.wave[i].enemies.size(); j++)
		{
			std::string std3 = "Wave_" + std::to_string(i + 1) + "_Enemy_" + std::to_string(j + 1) + "_Name";
			std::string std4 = "Wave_" + std::to_string(i + 1) + "_Enemy_" + std::to_string(j + 1) + "_Number";

			SetCharValue(waveInfo, std3.c_str(), _waveInfo.wave[i].enemies[j].name.c_str(), &xmlDoc);
			SetIntValue(waveInfo, std4.c_str(), _waveInfo.wave[i].enemies[j].number, &xmlDoc);
		}
	}

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}

ParticlesSystemDefinition TemplateManager::LoadParticle(const std::string& name)
{
	std::string filePath = PARTICLE + name + EXTENTION;

	ParticlesSystemDefinition particle = {};

	XMLDocument doc;

	// Load the XML file into the Doc instance
	doc.LoadFile(filePath.c_str());

	// Get root Element
	XMLElement* pRootElement = doc.RootElement();

	if (pRootElement == nullptr)
	{
		std::string str = "Invalid path or empty document : ";
		str = str + filePath;
		Logger::Error(str);
		return particle;
	}

	if (NULL != pRootElement)
	{
		//Get 'Characters' Child
		XMLElement* pParticle = pRootElement->FirstChildElement("Particle");
		if (NULL != pParticle)
		{
			particle.diffusal = GetIntValue(pParticle, "Diffusal", 0);

			particle.diffusalAngle.x = GetFloatValue(pParticle, "DiffusalAngleX", 0.f);
			particle.diffusalAngle.y = GetFloatValue(pParticle, "DiffusalAngleY", 0.f);
			particle.diffusalAngle.z = GetFloatValue(pParticle, "DiffusalAngleZ", 0.f);

			particle.diffusalDirection.x = GetFloatValue(pParticle, "DiffusalDirectionX", 0.f);
			particle.diffusalDirection.y = GetFloatValue(pParticle, "DiffusalDirectionY", 0.f);
			particle.diffusalDirection.z = GetFloatValue(pParticle, "DiffusalDirectionZ", 0.f);

			particle.pathTexture = GetCharValue(pParticle, "PathTexture", DEFAULT_TEXT);
			particle.m_name = GetCharValue(pParticle, "Name", "Default_name");

			particle.particleNumber = GetIntValue(pParticle, "ParticleNumber", 100);
		}
	}
	return particle;
}

void TemplateManager::WriteParticle(const ParticlesSystemDefinition& _particle, const std::string& name)
{
	std::string filePath = PARTICLE + name + EXTENTION;

	tinyxml2::XMLDocument xmlDoc;

	XMLElement* pRoot = xmlDoc.NewElement("file");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* particle = xmlDoc.NewElement("Particle");
	particle->SetText("\n");
	pRoot->InsertFirstChild(particle);


	SetIntValue(particle, "Diffusal", _particle.diffusal, &xmlDoc);
	
	SetFloatValue(particle, "DiffusalAngleX", _particle.diffusalAngle.x, &xmlDoc);
	SetFloatValue(particle, "DiffusalAngleY", _particle.diffusalAngle.y, &xmlDoc);
	SetFloatValue(particle, "DiffusalAngleZ", _particle.diffusalAngle.z, &xmlDoc);

	SetFloatValue(particle, "DiffusalDirectionX", _particle.diffusalDirection.x, &xmlDoc);
	SetFloatValue(particle, "DiffusalDirectionY", _particle.diffusalDirection.y, &xmlDoc);
	SetFloatValue(particle, "DiffusalDirectionZ", _particle.diffusalDirection.z, &xmlDoc);

	SetCharValue(particle, "PathTexture", _particle.pathTexture.c_str(), &xmlDoc);
	SetCharValue(particle, "Name", _particle.m_name.c_str(), &xmlDoc);

	SetIntValue(particle, "ParticleNumber", _particle.particleNumber, &xmlDoc);

	xmlDoc.SaveFile(filePath.c_str());

	std::string strInfo = filePath;
	strInfo = strInfo + " Saved";
	Logger::Info(strInfo);
}