/*
 * CCallbackBase.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once
#include "../GameConstants.h"

#define RETURN_IF_NOT_BATTLE(X) if(!duringBattle()) {logGlobal->error("%s called when no battle!", __FUNCTION__); return X; }

class IBattleInfo;
class BattleInfo;
class CBattleInfoEssentials;


//Basic class for various callbacks (interfaces called by players to get info about game and so forth)
class DLL_LINKAGE CCallbackBase
{
	const IBattleInfo * battle; //battle to which the player is engaged, nullptr if none or not applicable

	const IBattleInfo * getBattle() const;

protected:
	boost::optional<PlayerColor> player; // not set gives access to all information, otherwise callback provides only information "visible" for player

	CCallbackBase(boost::optional<PlayerColor> Player);
	CCallbackBase();

	void setBattle(const IBattleInfo * B);
	bool duringBattle() const;

public:
	boost::optional<PlayerColor> getPlayerID() const;

	friend class CBattleInfoEssentials;
};

