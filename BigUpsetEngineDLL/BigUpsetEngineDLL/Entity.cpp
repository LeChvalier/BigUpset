#include "Entity.h"
#include "TemplateManager.h"
#include "Scene.h"
#include "SoundManager.h"

#include "Debug.h"

#include <cassert>

UnitEntity::UnitEntity(UnitDefinition& _definition)
{
	definition = _definition;
	switch (_definition.scriptType)
	{
	case ST_BASIC_ALLIED_TANK: script = new AlliedScript(); break;
	case ST_BASIC_IA_ENEMY_TANK: script = new EnemyScript(); break;
	case ST_MINE: script = new MineScript(); break;
	case ST_TURRET: script = new TurretScript(); break;
	case ST_NOSCRIPT: script = new Script(); break;
	case ST_HEALER: script = new HealerScript(); break;
	}
}

DecorEntity::DecorEntity(DecorDefinition& _definition)
{
	definition = &_definition;

}

void UnitEntity::Update(Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem)
{
	if (script)
		script->Update(*this, scene, CharaDefinitions, BuildDefinitions, soundmanager, economySystem);
}

void UnitDefinition::Load()
{
	return;
}

void UnitDefinition::Save()
{
	/*std::string str = "..\\Game\\templates\\characters\\" + m_name + ".bup"; */
	if (m_type == UT_CHARACTER)
		TemplateManager::WriteCharacter(*this, m_name);
	else if (m_type == UT_BUILDING)
		TemplateManager::WriteBuilding(*this, m_name);
}

void DecorDefinition::Save()
{
	TemplateManager::WriteDecor(*this, m_name);
}

void ParticlesSystemDefinition::Save()
{
	TemplateManager::WriteParticle(*this, m_name);
}

UnitDefinition& GetUnitDefFromId(std::list<UnitDefinition>& units, int id)
{
	for (UnitDefinition& unit : units)
	{
		if (unit.m_id == id)
			return unit;
	}
	assert(false);

	// Deadly code becaus of assert
	UnitDefinition* null = new UnitDefinition();
	return *null;
}

UnitDefinition& GetUnitDefFromName(std::list<UnitDefinition>& units, std::string name)
{
	for (UnitDefinition& unit : units)
	{
		if (unit.m_name == name)
			return unit;
	}
	assert(false);

	// Deadly code becaus of assert
	UnitDefinition* null = new UnitDefinition();
	return *null;
}

#pragma region SCRIPT UNITS FUNCTIONS

void Script::DestroyUnit(std::vector<UnitEntity>& characterEntities, UnitEntity& unit, int index)
{
	for (auto it = characterEntities.begin(); it != characterEntities.end();)
	{
		UnitEntity& entity = *it;

		if (entity.gameID == index)
		{
			it = characterEntities.erase(it);
		}
		else
			it++;
	}
	//characterEntities.erase(characterEntities.begin() + index);
}

void Script::AddLife(UnitEntity& self, int _life)
{
	self.definition.m_life += _life;
}

void Script::SetLife(UnitEntity& self, int _life)
{
	self.definition.m_life = _life;
}

static UnitEntity* FindUnitById(std::vector<UnitEntity>& charaEntities, int id)
{
	for (UnitEntity& unit : charaEntities)
	{
		if (unit.gameID == id)
		{
			return &unit;
		}
	}
	return nullptr;
}

void Script::AIStateMachine(UnitEntity& self, std::vector<UnitEntity>& characterEntities, std::vector<UnitEntity>& BuildEntities, SoundManager& soundmanager, EconomySystem& economySystem, Scene* scene)
{
	if (self.definition.m_side == AISideType::ALLIED || self.definition.m_side == AISideType::NEUTRAL)
		return;

	// check if enemy units
	if (!characterEntities.empty() || !BuildEntities.empty())
	{
		float distance = 1000000;
		if (!self.definition.m_hasTarget || m_targetID == -1)
		{
			for (UnitEntity& chara : characterEntities)
			{
				if (distance > GetDistance(self.position, chara.position) && chara.definition.m_side == AISideType::ALLIED)
				{
					distance = GetDistance(self.position, chara.position);
					m_targetID = chara.gameID;
					self.definition.m_hasTarget = true;
				}
			}

			for (UnitEntity& build : BuildEntities)
			{
				if (distance > GetDistance(self.position, build.position) && build.definition.m_side == AISideType::ALLIED)
				{
					distance = GetDistance(self.position, build.position);
					m_targetID = build.gameID;
					self.definition.m_hasTarget = true;
				}
			}
		}
		else if (self.definition.m_hasTarget || m_targetID != -1)
		{
			UnitEntity* charaTarget = FindUnitById(characterEntities, m_targetID);
			UnitEntity* buildTtarget = FindUnitById(BuildEntities, m_targetID);

			if (charaTarget != nullptr)
			{
				Attack(self, *charaTarget, characterEntities, m_targetID, soundmanager, economySystem, scene);
			}
			else if (buildTtarget != nullptr)
			{
				Attack(self, *buildTtarget, BuildEntities, m_targetID, soundmanager, economySystem, scene);
			}
			else
				self.definition.m_hasTarget = false;
		}
	}
}

void Script::Move(UnitEntity& self, const float3& newPos)
{
	if (CheckDistance(self.position, newPos))
		return;

	self.vectorDir = (newPos - self.position).Normalize();

	// Keep in mind z axis is inverted
	if (self.vectorDir.z <= 0.f)
		self.rotation.y = -acosf(self.vectorDir.x) - Math::PI / 2.f;
	else
		self.rotation.y = acosf(self.vectorDir.x) - Math::PI / 2.f;

	self.position = self.position + m_timer.deltaTime * self.definition.m_speed * self.vectorDir;
}

bool Script::CheckDistance(const float3& self, const float3& target, const float& targetNear)
{
	float distanceX = (target.x - self.x) * (target.x - self.x);
	float distanceY = (target.y - self.y) * (target.y - self.y);
	float distanceZ = (target.z - self.z) * (target.z - self.z);

	if (sqrtf(distanceX + distanceY + distanceZ) < targetNear)
	{
		return true;
	}
	else
		return false;
}

float Script::GetDistance(const float3& self, const float3& target)
{
	float distanceX = (target.x - self.x) * (target.x - self.x);
	float distanceY = (target.y - self.y) * (target.y - self.y);
	float distanceZ = (target.z - self.z) * (target.z - self.z);

	return sqrtf(distanceX + distanceY + distanceZ);
}

void Script::Attack(UnitEntity& self, UnitEntity& aiEnemy, std::vector<UnitEntity>& characterEntities, int index, SoundManager& soundmanager, EconomySystem& economySystem, Scene* scene)
{
	if (self.definition.m_side == aiEnemy.definition.m_side || aiEnemy.definition.m_side == AISideType::NEUTRAL)
		return;

	if (self.definition.m_hasTarget)
	{
		if (!CheckDistance(self.position, aiEnemy.position, (float)self.definition.m_attackRange))
		{
			Move(self, aiEnemy.position);
			return;
		}
		//aiEnemy.definition.m_isAttacked = true;

		if (self.definition.m_canAttack)
		{
			aiEnemy.definition.m_life -= self.definition.m_damage;
			self.definition.m_canAttack = false;
			soundmanager.PlaySound(self.definition.m_shotSound, 0.2f);

			self.vectorDir = (aiEnemy.position - self.position).Normalize();

			// Keep in mind z axis is inverted
			if (self.vectorDir.z <= 0.f)
				self.rotation.y = -acosf(self.vectorDir.x) - Math::PI / 2.f;
			else
				self.rotation.y = acosf(self.vectorDir.x) - Math::PI / 2.f;

			float3 offset = {0.f, 0.25f, 0.f};
			scene->NewParticlesSystem(self.definition.m_attackParticle, self.position + offset, self.vectorDir);
			
			//Enemy is not attack, set bool to true and set the enemyAttackID to this unitID
			if (!aiEnemy.definition.m_isAttacked && !aiEnemy.definition.m_hasOrder && !aiEnemy.definition.m_hasTarget)
			{
				aiEnemy.definition.m_isAttacked = true;
				aiEnemy.definition.m_enemyID = self.gameID;
			}

			// Destroy if the enemy died
			if (aiEnemy.definition.m_life <= 0)
			{
				soundmanager.PlaySound(self.definition.m_dieSound, 0.2f);

				scene->NewParticlesSystem(self.definition.m_dieParticle, aiEnemy.position);

				economySystem.AddMoney(aiEnemy.definition.m_reward);
     			DestroyUnit(characterEntities, aiEnemy, index);
				self.definition.m_hasTarget = false;
			}
		}
		else if (!self.definition.m_canAttack && !m_timer.launchTimer)
		{
			m_timer.Start();
			m_timer.launchTimer = true;
		}

		if (m_timer.launchTimer && m_timer.GetDuration() > 0.30f)
		{
			self.definition.m_canAttack = true;;
			m_timer.launchTimer = false;
		}
	}
}

void Script::Healing(UnitEntity& self)
{
	//if (!CheckDistance(self.position, aiEnemy.position, (float)self.definition->m_escapeRange))
	//{
	//	float3 newPos = self.position -= aiEnemy.position;
	//	Move(self, newPos);
	//	return;
	//}

	if (self.definition.m_life < self.definition.m_maxlife)
	{
		if (self.definition.m_canHeal)
		{
			self.definition.m_life += self.definition.m_healing;
			self.definition.m_canHeal = false;
		}

		if (!self.definition.m_canHeal && !m_timer.launchTimer)
		{
			m_timer.Start();
			m_timer.launchTimer = true;
		}

		if (m_timer.launchTimer && m_timer.GetDuration() > 0.40f)
		{
			self.definition.m_canHeal = true;
			m_timer.launchTimer = false;
		}

		if (self.definition.m_life > self.definition.m_maxlife)
			self.definition.m_life = self.definition.m_maxlife;
	}
}

void Script::CheckLife(UnitEntity& self)
{
	if (self.definition.m_life <= 0)
		self.~UnitEntity();
}

void Script::OnClick(UnitEntity& selectedUnit, float3 pos, int pickedID, std::vector<UnitEntity>& charaEntities, std::vector<UnitEntity>& buildEntities)
{
	return;
}

#pragma endregion

#pragma region SCRIPT BUILDINGS FUNCTIONS
void Script::HealingUnits(UnitEntity& self, std::vector<UnitEntity>& allied)
{
	for (int i = 0; i < allied.size(); i++)
	{
		if (CheckDistance(self.position, allied[i].position, self.definition.m_healingRange) || self.definition.m_side != allied[i].definition.m_side)
			return;
		else
		{
			Healing(self);
		}
	}
}

#pragma endregion


#pragma region EnemyScript

void EnemyScript::Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem)
{
	AIStateMachine(self, scene->characterEntities, scene->buildingEntities, soundmanager, economySystem, scene);
}

#pragma endregion

#pragma region AllyScript

void AlliedScript::OnClick(UnitEntity& selectedUnit, float3 pos, int pickedID, std::vector<UnitEntity>& charaEntities, std::vector<UnitEntity>& buildEntities)
{
	UnitEntity* pickedUnit = FindUnitById(charaEntities, pickedID);
	UnitEntity* pickedBuilding = FindUnitById(buildEntities, pickedID);

	if (pickedUnit)
	{
		if (pickedUnit->definition.m_side == AISideType::ENEMY)
		{
			m_targetID = pickedID;
			selectedUnit.definition.m_hasTarget = true;
		}
	}
}

bool AlliedScript::IsSamePos(float3 pos, float3 nextPos)
{
	float3 distance = nextPos - pos;
	if (distance.Magnitude() < 0.1f)
		return true;

	return false;
}

void AlliedScript::Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem)
{
	if (m_targetID != -1 && self.definition.m_hasTarget)
	{
		UnitEntity* target = FindUnitById(scene->characterEntities, m_targetID);
		if (target != nullptr)
		{
			Attack(self, *target, scene->characterEntities, m_targetID, soundmanager, economySystem, scene);
		}
		else
			self.definition.m_hasTarget = false;
	}

	if (self.definition.m_hasOrder)
	{
		if (!IsSamePos(self.position, self.definition.movePos))
		{
			Move(self, self.definition.movePos);
		}
		else
		{
			self.definition.m_hasOrder = false;
			self.definition.movePos = {};
		}
	}
	else if (self.definition.m_isAttacked && !self.definition.m_hasOrder && !self.definition.m_hasTarget)
	{
		self.definition.m_hasTarget = true;
		m_targetID = self.definition.m_enemyID;

		self.definition.m_isAttacked = false;
	}
}

#pragma endregion

#pragma region MineScript

void MineScript::Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem)
{
	int index = 0;

	for (int i = 0; i < scene->characterEntities.size(); i++)
		if (self.definition.m_id == scene->characterEntities[i].definition.m_id)
			index = i;

	for (int i = 0; i < scene->characterEntities.size(); i++)
		if (!CheckDistance(self.position, scene->characterEntities[i].position, self.definition.m_attackRange) || self.definition.m_id == scene->characterEntities[i].definition.m_id)
			return;
		else
		{
			scene->characterEntities[i].definition.m_life -= self.definition.m_damage;

			if (scene->characterEntities[i].definition.m_life <= 0)
			{
				DestroyUnit(scene->characterEntities, scene->characterEntities[i], i);
				DestroyUnit(scene->characterEntities, self, index - 1);
			}
			else
			{
				DestroyUnit(scene->characterEntities, self, index);
			}

		}
}

#pragma endregion

#pragma region TurretScript

void TurretScript::Attack(UnitEntity& self, UnitEntity& aiEnemy, std::vector<UnitEntity>& characterEntities, int index, SoundManager& soundmanager, EconomySystem& economySystem, Scene* scene)
{
	if (self.definition.m_canAttack)
	{
		aiEnemy.definition.m_life -= self.definition.m_damage;
		self.definition.m_canAttack = false;
		soundmanager.PlaySound(self.definition.m_shotSound, 0.2f);

		self.vectorDir = (aiEnemy.position - self.position).Normalize();

		// Keep in mind z axis is inverted
		if (self.vectorDir.z <= 0.f)
			self.rotation.y = -acosf(self.vectorDir.x) - Math::PI / 2.f;
		else
			self.rotation.y = acosf(self.vectorDir.x) - Math::PI / 2.f;

		float3 offset = { 0.f, 0.25f, 0.f };
		scene->NewParticlesSystem(self.definition.m_attackParticle, self.position + offset, self.vectorDir);

		// Destroy if the enemy died
		if (aiEnemy.definition.m_life <= 0)
		{
			soundmanager.PlaySound(self.definition.m_dieSound, 0.2f);
			economySystem.AddMoney(aiEnemy.definition.m_reward);
			scene->NewParticlesSystem(self.definition.m_dieParticle, aiEnemy.position);

			DestroyUnit(characterEntities, aiEnemy, index);
		}
	}
	else if (!self.definition.m_canAttack && !m_timer.launchTimer)
	{
		m_timer.Start();
		m_timer.launchTimer = true;
	}

	if (m_timer.launchTimer && m_timer.GetDuration() > 0.30f)
	{
		self.definition.m_canAttack = true;;
		m_timer.launchTimer = false;
	}
}

void TurretScript::Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem)
{
	for (UnitEntity& unit : scene->characterEntities)
	{
		if (unit.definition.m_side == AISideType::ENEMY)
		{
			if (CheckDistance(self.position, unit.position, self.definition.m_attackRange))
			{
				m_targetID = unit.gameID;
			}
			else
				m_targetID = -1;

			UnitEntity* target = FindUnitById(scene->characterEntities, m_targetID);
			if (target != nullptr)
			{
				Attack(self, *target, scene->characterEntities, m_targetID, soundmanager, economySystem, scene);
			}
		}
	}
}

#pragma endregion

#pragma region HealerScript

void HealerScript::Update(UnitEntity& self, Scene* scene, std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, SoundManager& soundmanager, EconomySystem& economySystem)
{
	if (self.definition.m_canAttack)
	{
		for (UnitEntity& chara : scene->characterEntities)
		{
			if (CheckDistance(self.position, chara.position, self.definition.m_attackRange) && chara.definition.m_side == AISideType::ALLIED && chara.definition.m_life < chara.definition.m_maxlife)
			{
				chara.script->AddLife(chara, self.definition.m_damage);
				self.definition.m_canAttack = false;
				if (chara.definition.m_life > chara.definition.m_maxlife)
					chara.definition.m_life = chara.definition.m_maxlife;
				soundmanager.PlaySound(self.definition.m_shotSound, 0.2f);

				float3 offset = { 0.f, 0.25f, 0.f };
				scene->NewParticlesSystem(self.definition.m_attackParticle , self.position + offset);
			}
		}
	}
	else if (!self.definition.m_canAttack && !m_timer.launchTimer)
	{
		m_timer.Start();
		m_timer.launchTimer = true;
	}

	if (m_timer.launchTimer && m_timer.GetDuration() > self.definition.m_attackSpeed)
	{
		self.definition.m_canAttack = true;;
		m_timer.launchTimer = false;
	}
}

#pragma endregion