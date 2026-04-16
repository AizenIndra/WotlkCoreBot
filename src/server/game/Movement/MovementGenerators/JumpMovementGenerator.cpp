/*
 * Copyright (C) 2016-2019 AtieshCore <https://at-wow.org/>
 * Ported to AzerothCore: MovementGeneratorMedium API, JUMP_MOTION_TYPE.
 */

#include "JumpMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Player.h"
#include "Unit.h"

template<class T>
JumpMovementGenerator<T>::JumpMovementGenerator(uint32 id, float x, float y, float z, float o, float speedXY, float parabolicHeight, bool hasOrientation, bool orientationFixed, Unit const* facingTarget)
    : _movementId(id), _x(x), _y(y), _z(z), _o(o), _speedXY(speedXY), _parabolicHeight(parabolicHeight), _hasOrientation(hasOrientation), _orientationFixed(orientationFixed), _facingTarget(facingTarget), _recalculateSpeed(false)
{
}

template<class T>
void JumpMovementGenerator<T>::DoInitialize(T* owner)
{
    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting())
    {
        _recalculateSpeed = true;
        return;
    }

    if (!owner->IsStopped())
        owner->StopMoving();

    Movement::MoveSplineInit init(owner);
    init.MoveTo(_x, _y, _z, false);
    init.SetParabolic(_parabolicHeight, 0);
    if (_orientationFixed)
        init.SetOrientationFixed(true);
    init.SetVelocity(_speedXY);
    if (_hasOrientation)
        init.SetFacing(_o);
    else if (_facingTarget)
        init.SetFacing(_facingTarget);

    owner->SetIsJumping(true);
    init.Launch();
    owner->UpdateSplinePosition();
}

template<class T>
void JumpMovementGenerator<T>::DoReset(T* owner)
{
    DoInitialize(owner);
}

template<class T>
bool JumpMovementGenerator<T>::DoUpdate(T* owner, uint32 /*diff*/)
{
    if (!owner)
        return false;

    if (owner->IsMovementPreventedByCasting())
    {
        owner->StopMoving();
        return true;
    }

    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE))
    {
        owner->StopMoving();
        return true;
    }

    if (_recalculateSpeed && !owner->movespline->Finalized())
    {
        _recalculateSpeed = false;
        Movement::MoveSplineInit init(owner);
        init.MoveTo(_x, _y, _z, false);
        init.SetParabolic(_parabolicHeight, 0);
        if (_orientationFixed)
            init.SetOrientationFixed(true);
        init.SetVelocity(_speedXY);
        if (_hasOrientation)
            init.SetFacing(_o);
        else if (_facingTarget)
            init.SetFacing(_facingTarget);
        init.Launch();
    }

    return !owner->movespline->Finalized();
}

template<class T>
void JumpMovementGenerator<T>::DoFinalize(T* owner)
{
    owner->SetIsJumping(false);

    if (owner->movespline->Finalized())
        MovementInform(owner);
}

template<class T>
void JumpMovementGenerator<T>::MovementInform(T* /*unit*/)
{
}

template<>
void JumpMovementGenerator<Creature>::MovementInform(Creature* unit)
{
    if (unit->AI())
        unit->AI()->MovementInform(JUMP_MOTION_TYPE, _movementId);

    if (Unit* summoner = unit->GetCharmerOrOwner())
    {
        if (UnitAI* AI = summoner->GetAI())
            AI->SummonMovementInform(unit, JUMP_MOTION_TYPE, _movementId);
    }
    else if (TempSummon* tempSummon = unit->ToTempSummon())
        if (Unit* summoner = tempSummon->GetSummonerUnit())
            if (UnitAI* AI = summoner->GetAI())
                AI->SummonMovementInform(unit, JUMP_MOTION_TYPE, _movementId);
}

template JumpMovementGenerator<Player>::JumpMovementGenerator(uint32, float, float, float, float, float, float, bool, bool, Unit const*);
template JumpMovementGenerator<Creature>::JumpMovementGenerator(uint32, float, float, float, float, float, float, bool, bool, Unit const*);
template void JumpMovementGenerator<Player>::DoInitialize(Player*);
template void JumpMovementGenerator<Creature>::DoInitialize(Creature*);
template void JumpMovementGenerator<Player>::DoReset(Player*);
template void JumpMovementGenerator<Creature>::DoReset(Creature*);
template bool JumpMovementGenerator<Player>::DoUpdate(Player*, uint32);
template bool JumpMovementGenerator<Creature>::DoUpdate(Creature*, uint32);
template void JumpMovementGenerator<Player>::DoFinalize(Player*);
template void JumpMovementGenerator<Creature>::DoFinalize(Creature*);
template void JumpMovementGenerator<Player>::MovementInform(Player*);
template void JumpMovementGenerator<Creature>::MovementInform(Creature*);
