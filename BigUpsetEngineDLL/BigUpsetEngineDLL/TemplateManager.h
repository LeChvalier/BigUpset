#pragma once

#include <string>

struct ParticlesSystemDefinition;
struct DecorDefinition;
struct UnitDefinition;
struct SoundInfo;
struct WaveInfo;
struct Map;

class Camera;
class EconomySystem;

namespace tinyxml2
{
	class XMLDocument;
};

class TemplateManager
{
public:
	static UnitDefinition LoadCharacter(const std::string& name);
	static void WriteCharacter(const UnitDefinition& _character, const std::string& name);

	static UnitDefinition LoadBuilding(const std::string& name);
	static void WriteBuilding(const UnitDefinition& _building, const std::string& name);

	static Camera LoadCamera(const std::string& name);
	static void WriteCamera(Camera camera, const std::string& name);


	static std::string LoadGeneral(const std::string& name);
	static void WriteGeneral(std::string gameName, const std::string& name);

	static EconomySystem LoadEconomySystem(const std::string& name);
	static void WriteEconomySystem(EconomySystem economySystem, const std::string& name);


	static Map LoadMap(const std::string& name);
	static void WriteMap(Map& _map, const std::string& name);

	static DecorDefinition LoadDecor(const std::string& name);
	static void WriteDecor(const DecorDefinition& _decor, const std::string& name);

	static SoundInfo LoadSoundInfo(const std::string& name);
	static void WriteSoundInfo(const SoundInfo& _soundInfo, const std::string& name);

	static WaveInfo LoadWaveInfo(const std::string& name);
	static void WriteWaveInfo(const WaveInfo& _waveInfo, const std::string& name);

	static ParticlesSystemDefinition LoadParticle(const std::string& name);
	static void WriteParticle(const ParticlesSystemDefinition& _particle, const std::string& name);

	static int GetIntValue(void* element, const char* name, int defaultValue);
	static float GetFloatValue(void* element, const char* name, float defaultValue);
	static const char* GetCharValue(void* element, const char* name, const char* defaultValue);

	static void SetIntValue(void* element, const char* name, int value, tinyxml2::XMLDocument* xmlDoc);
	static void SetFloatValue(void* element, const char* name, float value, tinyxml2::XMLDocument* xmlDoc);
	static void SetCharValue(void* element, const char* name, const char* value, tinyxml2::XMLDocument* xmlDoc);
};
