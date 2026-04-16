/*
 * Copyright (C) 2016-2019 AtieshCore <https://at-wow.org/>
 * Ported to AzerothCore: MovementGeneratorMedium API, JUMP_MOTION_TYPE.
 */

#ifndef ACORE_JUMPMOVEMENTGENERATOR_H
#define ACORE_JUMPMOVEMENTGENERATOR_H

#include "MovementGenerator.h"

class Unit;

template<class T>
class JumpMovementGenerator : public MovementGeneratorMedium<T, JumpMovementGenerator<T>>
{
public:
    JumpMovementGenerator(uint32 id, float x, float y, float z, float o, float speedXY, float parabolicHeight, bool hasOrientation, bool orientationFixed, Unit const* facingTarget = nullptr);

    void DoInitialize(T*);
    void DoReset(T*);
    bool DoUpdate(T*, uint32);
    void DoFinalize(T*);

    void unitSpeedChanged() { _recalculateSpeed = true; }

    MovementGeneratorType GetMovementGeneratorType() { return JUMP_MOTION_TYPE; }

    [[nodiscard]] uint32 GetId() const { return _movementId; }

private:
    void MovementInform(T*);

    uint32 _movementId;
    float _x, _y, _z, _o;
    float _speedXY;
    float _parabolicHeight;
    bool _hasOrientation;
    bool _orientationFixed;
    Unit const* _facingTarget;
    bool _recalculateSpeed;
};

#endif
