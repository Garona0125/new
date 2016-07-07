/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef SC_ACDATA_H
#define SC_ACDATA_H

#include "AnticheatMgr.h"

class AnticheatData
{
public:
	AnticheatData();

	void SetLastOpcode(uint32 opcode);
	uint32 GetLastOpcode() const;

	const MovementInfo& GetLastMovementInfo() const;
	void SetLastMovementInfo(MovementInfo& moveInfo);

	void SetPosition(float x, float y, float z, float o);
private:
	uint32 lastOpcode;
	MovementInfo lastMovementInfo;
};

#endif