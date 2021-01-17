/*
 * Dispel.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include "UnitEffect.h"

struct Bonus;
class CSelector;
class BonusList;
struct SetStackEffect;

namespace spells
{
namespace effects
{

class Dispel : public UnitEffect
{
public:
	Dispel();
	virtual ~Dispel();

	void apply(BattleStateProxy * battleState, RNG & rng, const Mechanics * m, const EffectTarget & target) const override;

protected:
	bool isValidTarget(const Mechanics * m, const battle::Unit * unit) const override;
	void serializeJsonUnitEffect(JsonSerializeFormat & handler) override final;

private:
	bool positive = false;
	bool negative = false;
	bool neutral = false;

	std::shared_ptr<BonusList> getBonuses(const Mechanics * m, const battle::Unit * unit) const;

	static bool mainSelector(const Bonus * bonus);
	void prepareEffects(SetStackEffect & pack, RNG & rng, const Mechanics * m, const EffectTarget & target, bool describe) const;
};

}
}
