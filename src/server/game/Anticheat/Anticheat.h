/*
 * Copyright (C) 2016-2019 AtieshCore <https://at-wow.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#ifndef _ANTICHEAT_H
#define _ANTICHEAT_H

#include "Common.h"

class Player;
struct MovementInfo;

class AC_GAME_API Anticheat
{
public:
    Anticheat(Player* player);
    ~Anticheat();

    void update(uint32 p_time);
    void punish(uint8 method);

    void resetFallingData(float z);
    void startWaitingLandOrSwimOpcode();
    void updateFallInformationIfNeed(float newZ);
    bool isWaitingLandOrSwimOpcode() const { return m_antiNoFallDmg; }
    bool isUnderLastChanceForLandOrSwimOpcode() const { return m_antiNoFallDmgLastChance; }
    void setSuccessfullyLanded() { m_antiNoFallDmgLastChance = false; m_antiNoFallDmg = false; }

    void setSkipOnePacketForASH(bool blinked) { m_skipOnePacketForASH = blinked; }
    bool isSkipOnePacketForASH() const { return m_skipOnePacketForASH; }
    void setJumpingbyOpcode(bool jump) { m_isjumping = jump; }
    bool isJumpingbyOpcode() const { return m_isjumping; }
    void setCanFlybyServer(bool canfly) { m_canfly = canfly; }
    bool isCanFlybyServer() const { return m_canfly; }

    bool underACKmount() const { return m_ACKmounted; }
    bool underACKRootUpd() const { return m_rootUpd; }

    void setUnderACKmount();
    void setRootACKUpd(uint32 delay);

    void setLastMoveClientTimestamp(uint32 timestamp) { lastMoveClientTimestamp = timestamp; }
    void setLastMoveServerTimestamp(uint32 timestamp) { lastMoveServerTimestamp = timestamp; }
    uint32 getLastMoveClientTimestamp() const { return lastMoveClientTimestamp; }
    uint32 getLastMoveServerTimestamp() const { return lastMoveServerTimestamp; }
    void updateMovementInfo(MovementInfo const& movementInfo);

    bool checkOnFlyHack();
    bool IsIllegalWaterWalkMovement(MovementInfo const& movementInfo) const;
    bool IsIllegalMegaJump(MovementInfo const& movementInfo) const;
    bool checkMovementInfo(MovementInfo const& movementInfo, bool jump);

    uint8 getLastMovementCheckFailure() const { return _movementCheckFailure; }

    std::string getDescriptionACForLogs(uint8 type, float param1 = 0.f, float param2 = 0.f) const;
    std::string getPositionACForLogs() const;

    void setReloadModelsDisplayTimer();

private:
    Player* pPlayer = nullptr;

    bool m_skipOnePacketForASH;
    bool m_isjumping;
    bool m_canfly;
    bool m_ACKmounted;
    bool m_rootUpd;
    bool m_antiNoFallDmg;
    bool m_antiNoFallDmgLastChance;
    uint32 m_mountTimer;
    uint32 m_rootUpdTimer;
    uint32 m_flyhackTimer;
    uint32 m_antiNoFallDmgTimer;
    uint32 m_reloadModelsDisplayTimer;

    uint32 lastMoveClientTimestamp;
    uint32 lastMoveServerTimestamp;

    uint8 _movementCheckFailure = 0;

    mutable float _megaJumpLastZspeed = 0.f;
    mutable float _megaJumpLastMaxZ = 0.f;
};

#endif // _ANTICHEAT_H
