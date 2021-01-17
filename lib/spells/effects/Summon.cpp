/*
 * Summon.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Summon.h"
#include "Registry.h"

#include "../ISpellMechanics.h"
#include "../../battle/CBattleInfoCallback.h"
#include "../../battle/Unit.h"
#include "../../NetPacks.h"
#include "../../serializer/JsonSerializeFormat.h"

#include "../../CHeroHandler.h"
#include "../../mapObjects/CGHeroInstance.h"

static const std::string EFFECT_NAME = "core:summon";

namespace spells
{
namespace effects
{

VCMI_REGISTER_SPELL_EFFECT(Summon, EFFECT_NAME);

Summon::Summon()
	: Effect(),
	creature(),
	permanent(false),
	exclusive(true)
{
}

Summon::~Summon() = default;

void Summon::adjustAffectedHexes(std::set<BattleHex> & hexes, const Mechanics * m, const Target & spellTarget) const
{
	//no hexes affected
}

void Summon::adjustTargetTypes(std::vector<TargetType> & types) const
{
	//any target type allowed
}

bool Summon::applicable(Problem & problem, const Mechanics * m) const
{
	if(!exclusive)
		return true;

	//check if there are summoned elementals of other type

	auto otherSummoned = m->cb->battleGetUnitsIf([m, this](const battle::Unit * unit)
	{
		return (unit->unitOwner() == m->getCasterColor())
			&& (unit->unitSlot() == SlotID::SUMMONED_SLOT_PLACEHOLDER)
			&& (!unit->isClone())
			&& (unit->creatureId() != creature);
	});

	if(!otherSummoned.empty())
	{
		auto elemental = otherSummoned.front();

		MetaString text;
		text.addTxt(MetaString::GENERAL_TXT, 538);

		auto caster = dynamic_cast<const CGHeroInstance *>(m->caster);
		if(caster)
		{
			text.addReplacement(caster->name);

			text.addReplacement(MetaString::CRE_PL_NAMES, elemental->creatureIndex());

			if(caster->type->sex)
				text.addReplacement(MetaString::GENERAL_TXT, 540);
			else
				text.addReplacement(MetaString::GENERAL_TXT, 539);

		}
		problem.add(std::move(text), Problem::NORMAL);
		return false;
	}

	return true;
}

void Summon::apply(BattleStateProxy * battleState, RNG & rng, const Mechanics * m, const EffectTarget & target) const
{
	//new feature - percentage bonus
	auto amount = m->applySpecificSpellBonus(m->calculateRawEffectValue(0, m->getEffectPower()));
	if(amount < 1)
	{
		battleState->complain("Summoning didn't summon any!");
		return;
	}

	BattleUnitsChanged pack;

	for(auto & dest : target)
	{
		if(dest.unitValue)
		{
			const battle::Unit * summoned = dest.unitValue;
			std::shared_ptr<battle::Unit> state = summoned->acquire();
			int64_t healthValue = amount * summoned->MaxHealth();
			state->heal(healthValue, EHealLevel::OVERHEAL, (permanent ? EHealPower::PERMANENT : EHealPower::ONE_BATTLE));
			pack.changedStacks.emplace_back(summoned->unitId(), UnitChanges::EOperation::RESET_STATE);
			state->save(pack.changedStacks.back().data);
		}
		else
		{
			battle::UnitInfo info;
			info.id = m->cb->battleNextUnitId();
			info.count = amount;
			info.type = creature;
			info.side = m->casterSide;
			info.position = dest.hexValue;
			info.summoned = !permanent;

			pack.changedStacks.emplace_back(info.id, UnitChanges::EOperation::ADD);
			info.save(pack.changedStacks.back().data);
		}
	}

	if(!pack.changedStacks.empty())
		battleState->apply(&pack);
}

EffectTarget Summon::filterTarget(const Mechanics * m, const EffectTarget & target) const
{
	return target;
}

void Summon::serializeJsonEffect(JsonSerializeFormat & handler)
{
	handler.serializeId("id", creature, CreatureID());
	handler.serializeBool("permanent", permanent, false);
	handler.serializeBool("exclusive", exclusive, true);
}

EffectTarget Summon::transformTarget(const Mechanics * m, const Target & aimPoint, const Target & spellTarget) const
{
	auto sameSummoned = m->cb->battleGetUnitsIf([m, this](const battle::Unit * unit)
	{
		return (unit->unitOwner() == m->getCasterColor())
			&& (unit->unitSlot() == SlotID::SUMMONED_SLOT_PLACEHOLDER)
			&& (!unit->isClone())
			&& (unit->creatureId() == creature)
			&& (unit->alive());
	});

	EffectTarget effectTarget;

	if(sameSummoned.empty())
	{
		BattleHex hex = m->cb->getAvaliableHex(creature, m->casterSide);
		if(!hex.isValid())
			logGlobal->error("No free space to summon creature!");
		else
			effectTarget.emplace_back(hex);
	}
	else
	{
		effectTarget.emplace_back(sameSummoned.front());
	}

	return effectTarget;
}


}
}
