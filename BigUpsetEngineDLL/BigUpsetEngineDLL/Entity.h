#pragma once
#include <iostream>
#include <string>
#include <list>

#include "TemplateManager.h"
#include "ObjectPointer.h"
#include "Objects.h"
#include "Timer.h"
#include "Define.h"
#include "Economy.h"
#include "ParticlesSystem.h"

class Scene;
class SoundManager;

enum UnitType
{
	UT_CHARACTER,
	UT_BUILDING
};

enum ScriptType
{
	ST_BASIC_ALLIED_TANK,
	ST_BASIC_IA_ENEMY_TANK,
	ST_MINE,
	ST_TURRET,
	ST_NOSCRIPT,
	ST_HEALER,
	COUNT
};

enum AISideType
{
	ALLIED,
	ENEMY,
	NEUTRAL,
	COUNTER
};



class ScriptHelper
{
public:
	static std::string GetNameScript(ScriptType type)
	{
		switch (type)
		{
		case ST_BASIC_ALLIED_TANK:   return "Basic_Allied_Tank";   break;
		case ST_BASIC_IA_ENEMY_TANK:    return "Basic_IA_Enemy_Tank";    break;
		case ST_MINE:     return "Mine";     break;
		case ST_TURRET:   return "Turret";   break;
		case ST_NOSCRIPT: return "NoScript"; break;
		case ST_HEALER: return "Healer"; break;
		}
		return "";
	}

	static std::string GetNameSide(AISideType type)
	{
		switch (type)
		{
		case ALLIED:   return "Allied";   break;
		case NEUTRAL:  return "Neutral";  break;
		case ENEMY:    return "Enemy";    break;
		}
		return "";
	}
};

struct ParticlesSystemDefinition
{
	int m_id = 0;
	std::string m_name = "Default name";

	bool diffusal = false;
	float3 diffusalDirection = { 1.f, 0.f, 0.f };
	float3 diffusalAngle = { Math::PI / 4.f, Math::PI / 4.f, Math::PI / 4.f };

	int particleNumber = 10;

	std::string pathTexture = DEFAULT_TEXT;

	void Save();
};

struct UnitDefinition
{
	AISideType m_side = AISideType::NEUTRAL;
	ScriptType scriptType = ST_NOSCRIPT;
	UnitType m_type = UT_CHARACTER;

	int m_id = 0;
	std::string m_name = "Default name";

	int m_attackRange = 0;
	int m_attackSpeed = 0;
	int m_damage      = 0;
	int m_enemyID     = 0;
	int m_healing     = 0;
	int m_life        = 0;
	int m_maxlife     = 0;

	int m_price = 0;
	int m_reward = 0;

	bool m_canAttack  = true;
	bool m_canHeal    = true;
	bool m_isAttacked = false;
	bool m_hasTarget  = false;
	bool m_hasOrder   = false;

	std::string pathMesh    = DEFAULT_MESH;
	std::string pathTexture = DEFAULT_TEXT;

	// Character specific properties
	int m_speed = 0;
	float3 movePos = { 0.f,0.f,0.f };

	std::string m_shotSound = DEFAULT_SOUND;
	std::string m_dieSound = DEFAULT_SOUND;

	// Building specific properties
	bool  m_canCreateCharacter = false;
	bool  m_mainBuilding       = false;
	int   m_cooldownCreateCharacter = 0;
	float m_healingRange            = 0;

	int m_IdUnitDefCreateable = 0;

	int m_IdAttackParticle = 0;
	int m_IdDieParticle = 0;

	ParticlesSystemDefinition m_attackParticle;
	ParticlesSystemDefinition m_dieParticle;

	Model model = {}; // Run-time only (not serialized)

	void Load();
	void Save();
};

struct DecorDefinition
{
	int m_id = 0;
	std::string m_name = "Default name";

	std::string pathMesh = DEFAULT_MESH;
	std::string pathTexture = DEFAULT_TEXT;

	Model model = {}; // Run-time only (not serialized)

	void Load();
	void Save();
};

class Entity
{
public:
	virtual ~Entity() = default;

	float3 position = { 0.f,0.f,0.f };
	float3 rotation = { 0.f,0.f,0.f };
	float3 scale    = { 1.f,1.f,1.f };

	virtual void Update(Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) {}
};

// Class for none gameplay entities (trees, rocks)
class DecorEntity : public Entity
{
public:
	DecorEntity(DecorDefinition& _definition);

	DecorDefinition* definition;
	
	// not very usefull
	void Update(Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override {};
};


class Script;


class UnitEntity : public Entity
{
public:
	UnitEntity(UnitDefinition& _definition);

	~UnitEntity()
	{
		/*if(script != nullptr)
			delete script;*/
	};
	
	int gameID = -1;

	int targetID = -1;

	UnitDefinition definition;

	Script* script = nullptr;

	float3 vectorDir = {0.f, 0.f, 0.f};

	void Update(Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override;
};

class Script
{
private:
	Timer m_timer;
public:
	virtual void Test(float3& pos) {};

	void DestroyUnit(std::vector<UnitEntity>& characterEntities, UnitEntity& unit, int index);
	
	void AddLife(UnitEntity& self, int _life);
	void SetLife(UnitEntity& self, int _life);

	// UNITS FUNCTIONS //
	void AIStateMachine(UnitEntity& self, std::vector<UnitEntity>& characterEntities, std::vector<UnitEntity>& BuildEntities, SoundManager& soundmanager, EconomySystem& economySystem, Scene* scene);
	void Move(UnitEntity& self, const float3& newPos);
	bool CheckDistance(const float3& self, const float3& target, const float& targetNear = 0.1f);
	float GetDistance(const float3& self, const float3& target);
	virtual void Attack(UnitEntity& self, UnitEntity& aiEnemy, std::vector<UnitEntity>& characterEntities, int index, SoundManager& soundmanager, EconomySystem& economySystem, Scene* scene);
	void Healing(UnitEntity& self);
	void CheckLife(UnitEntity& self);
	virtual void OnClick(UnitEntity& selectedUnit, float3 pos, int pickedID, std::vector<UnitEntity>& charaEntities, std::vector<UnitEntity>& buildEntities);

	// BUILDING FUNCTIONS //
	void HealingUnits(UnitEntity& self, std::vector<UnitEntity>& allied);

	virtual void Update(UnitEntity& self , Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) {}
	int m_targetID = -1;
};

class EnemyScript : public Script
{
public:
	void Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override;
};

class AlliedScript : public Script
{
public:
	//void SetNewTarget(int EnemyID);
	int m_targetID = -1;
	bool IsSamePos(float3 Pos, float3 NextPos);
	void Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override;
	void OnClick(UnitEntity& selectedUnit, float3 pos, int pickedID, std::vector<UnitEntity>& charaEntities, std::vector<UnitEntity>& buildEntities) override;


private:
};

class MineScript : public Script
{
public:
	void Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override;
};

class TurretScript : public Script
{
private:
	Timer m_timer;

public:
	void Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override;
	void Attack(UnitEntity& self, UnitEntity& aiEnemy, std::vector<UnitEntity>& characterEntities, int index, SoundManager& soundmanager, EconomySystem& economySystem, Scene* scene) override;
};

class HealerScript : public Script
{
private:
	Timer m_timer;
public:
	void Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem) override;
};

UnitDefinition& GetUnitDefFromId(std::list<UnitDefinition>& units, int id);
UnitDefinition& GetUnitDefFromName(std::list<UnitDefinition>& units, std::string name);
