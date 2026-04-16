/*
 * Copyright (C) 2019 AtieshCore <https://at-wow.org/>
 * Ported to AzerothCore (MovementGeneratorMedium, no Trinity flags).
 */

#include "ChargeMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "PathGenerator.h"
#include "Player.h"
#include <G3D/g3dmath.h>

template<class T>
ChargeMovementGenerator<T>::ChargeMovementGenerator(uint32 id, float x, float y, float z, bool generatePath, float speed, std::optional<float> finalOrient, const Movement::PointsArray* path, ObjectGuid chargeTargetGUID)
    : _movementId(id), _x(x), _y(y), _z(z), _speed(speed), _generatePath(generatePath), _forceDestination(false), _finalOrient(finalOrient), _chargeTargetGUID(chargeTargetGUID), _recalculateSpeed(false)
{
    if (path)
        _precomputedPath = *path;
}

template<class T>
void ChargeMovementGenerator<T>::DoInitialize(T* owner)
{
    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting())
    {
        _recalculateSpeed = true;
        return;
    }

    if (!owner->IsStopped())
        owner->StopMoving();

    owner->AddUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CHARGING);

    Movement::MoveSplineInit init(owner);

    if (_precomputedPath.size() > 2)
        init.MovebyPath(_precomputedPath);
    else if (_precomputedPath.size() == 2)
        init.MoveTo(_precomputedPath[1].x, _precomputedPath[1].y, _precomputedPath[1].z, true);
    else if (_generatePath)
    {
        PathGenerator path(owner);
        bool const result = path.CalculatePath(_x, _y, _z, _forceDestination);
        if (result && !(path.GetPathType() & PATHFIND_NOPATH) && path.GetPath().size() > 2)
        {
            _precomputedPath = path.GetPath();
            init.MovebyPath(_precomputedPath);
        }
        else
        {
            float tx = _x, ty = _y;
            if (G3D::fuzzyEq(owner->GetPositionX(), _x) && G3D::fuzzyEq(owner->GetPositionY(), _y))
            {
                tx += 0.2f * std::cos(owner->GetOrientation());
                ty += 0.2f * std::sin(owner->GetOrientation());
            }
            init.MoveTo(tx, ty, _z, true);
        }
    }
    else
    {
        float tx = _x, ty = _y;
        if (G3D::fuzzyEq(owner->GetPositionX(), _x) && G3D::fuzzyEq(owner->GetPositionY(), _y))
        {
            tx += 0.2f * std::cos(owner->GetOrientation());
            ty += 0.2f * std::sin(owner->GetOrientation());
        }
        init.MoveTo(tx, ty, _z, true);
    }

    if (_speed > 0.0f)
        init.SetVelocity(_speed);

    if (_finalOrient)
        init.SetFacing(*_finalOrient);

    init.Launch();
}

template<class T>
void ChargeMovementGenerator<T>::DoReset(T* owner)
{
    DoInitialize(owner);
}

template<class T>
bool ChargeMovementGenerator<T>::DoUpdate(T* owner, uint32 /*diff*/)
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
        if (!owner->HasUnitState(UNIT_STATE_CHARGING))
            owner->StopMoving();
        return true;
    }

    owner->AddUnitState(UNIT_STATE_ROAMING_MOVE);

    if (_recalculateSpeed && !owner->movespline->Finalized())
    {
        _recalculateSpeed = false;
        Movement::MoveSplineInit init(owner);

        if (_precomputedPath.size() > 1)
        {
            uint32 offset = std::min(uint32(owner->movespline->_currentSplineIdx()), uint32(_precomputedPath.size()));
            Movement::PointsArray::iterator offsetItr = _precomputedPath.begin();
            std::advance(offsetItr, offset);
            _precomputedPath.erase(_precomputedPath.begin(), offsetItr);
            _precomputedPath.insert(_precomputedPath.begin(), G3D::Vector3(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ()));

            if (_precomputedPath.size() > 2)
                init.MovebyPath(_precomputedPath);
            else if (_precomputedPath.size() == 2)
                init.MoveTo(_precomputedPath[1].x, _precomputedPath[1].y, _precomputedPath[1].z, true);
        }
        else
            init.MoveTo(_x, _y, _z, true);

        if (_speed > 0.0f)
            init.SetVelocity(_speed);

        if (_finalOrient)
            init.SetFacing(*_finalOrient);

        init.Launch();
    }

    return !owner->movespline->Finalized();
}

template<class T>
void ChargeMovementGenerator<T>::DoFinalize(T* owner)
{
    owner->ClearUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CHARGING);

    if (_chargeTargetGUID && _chargeTargetGUID == owner->GetTarget())
    {
        if (Unit* target = ObjectAccessor::GetUnit(*owner, _chargeTargetGUID))
            owner->Attack(target, true);
    }

    if (owner->movespline->Finalized())
        MovementInform(owner);
}

template<class T>
void ChargeMovementGenerator<T>::MovementInform(T* /*unit*/)
{
}

template<>
void ChargeMovementGenerator<Creature>::MovementInform(Creature* unit)
{
    if (unit->AI())
        unit->AI()->MovementInform(CHARGE_MOTION_TYPE, _movementId);

    if (Unit* summoner = unit->GetCharmerOrOwner())
    {
        if (UnitAI* AI = summoner->GetAI())
            AI->SummonMovementInform(unit, CHARGE_MOTION_TYPE, _movementId);
    }
    else if (TempSummon* tempSummon = unit->ToTempSummon())
        if (Unit* summoner = tempSummon->GetSummonerUnit())
            if (UnitAI* AI = summoner->GetAI())
                AI->SummonMovementInform(unit, CHARGE_MOTION_TYPE, _movementId);
}

template<class T>
ChargePathMovementGenerator<T>::ChargePathMovementGenerator(uint32 id, float x, float y, float z, Movement::PointsArray pathPoints, float speed, std::optional<float> finalOrient)
    : _movementId(id), _x(x), _y(y), _z(z), _pathPoints(std::move(pathPoints)), _speed(speed), _finalOrient(finalOrient), _recalculateSpeed(false)
{
}

template<class T>
void ChargePathMovementGenerator<T>::DoInitialize(T* owner)
{
    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting())
    {
        _recalculateSpeed = true;
        return;
    }

    if (!owner->IsStopped())
        owner->StopMoving();

    owner->AddUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CHARGING);

    Movement::MoveSplineInit init(owner);
    if (_pathPoints.size() > 1)
        init.MovebyPath(_pathPoints);
    else
        init.MoveTo(_x, _y, _z, true);

    if (_speed > 0.0f)
        init.SetVelocity(_speed);

    if (_finalOrient)
        init.SetFacing(*_finalOrient);

    init.Launch();
}

template<class T>
void ChargePathMovementGenerator<T>::DoReset(T* owner)
{
    DoInitialize(owner);
}

template<class T>
bool ChargePathMovementGenerator<T>::DoUpdate(T* owner, uint32 /*diff*/)
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
        if (!owner->HasUnitState(UNIT_STATE_CHARGING))
            owner->StopMoving();
        return true;
    }

    owner->AddUnitState(UNIT_STATE_ROAMING_MOVE);

    if (_recalculateSpeed && !owner->movespline->Finalized())
    {
        _recalculateSpeed = false;
        Movement::MoveSplineInit init(owner);
        if (_pathPoints.size() > 1)
            init.MovebyPath(_pathPoints);
        else
            init.MoveTo(_x, _y, _z, true);
        if (_speed > 0.0f)
            init.SetVelocity(_speed);
        if (_finalOrient)
            init.SetFacing(*_finalOrient);
        init.Launch();
    }

    return !owner->movespline->Finalized();
}

template<class T>
void ChargePathMovementGenerator<T>::DoFinalize(T* owner)
{
    owner->ClearUnitState(UNIT_STATE_ROAMING | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CHARGING);

    if (owner->movespline->Finalized())
        MovementInform(owner);
}

template<class T>
void ChargePathMovementGenerator<T>::MovementInform(T* /*unit*/)
{
}

template<>
void ChargePathMovementGenerator<Creature>::MovementInform(Creature* unit)
{
    if (unit->AI())
        unit->AI()->MovementInform(CHARGE_MOTION_TYPE, _movementId);

    if (Unit* summoner = unit->GetCharmerOrOwner())
    {
        if (UnitAI* AI = summoner->GetAI())
            AI->SummonMovementInform(unit, CHARGE_MOTION_TYPE, _movementId);
    }
    else if (TempSummon* tempSummon = unit->ToTempSummon())
        if (Unit* summoner = tempSummon->GetSummonerUnit())
            if (UnitAI* AI = summoner->GetAI())
                AI->SummonMovementInform(unit, CHARGE_MOTION_TYPE, _movementId);
}

template ChargeMovementGenerator<Player>::ChargeMovementGenerator(uint32, float, float, float, bool, float, std::optional<float>, const Movement::PointsArray*, ObjectGuid);
template ChargeMovementGenerator<Creature>::ChargeMovementGenerator(uint32, float, float, float, bool, float, std::optional<float>, const Movement::PointsArray*, ObjectGuid);
template void ChargeMovementGenerator<Player>::DoInitialize(Player*);
template void ChargeMovementGenerator<Creature>::DoInitialize(Creature*);
template void ChargeMovementGenerator<Player>::DoReset(Player*);
template void ChargeMovementGenerator<Creature>::DoReset(Creature*);
template bool ChargeMovementGenerator<Player>::DoUpdate(Player*, uint32);
template bool ChargeMovementGenerator<Creature>::DoUpdate(Creature*, uint32);
template void ChargeMovementGenerator<Player>::DoFinalize(Player*);
template void ChargeMovementGenerator<Creature>::DoFinalize(Creature*);
template void ChargeMovementGenerator<Player>::MovementInform(Player*);
template void ChargeMovementGenerator<Creature>::MovementInform(Creature*);

template ChargePathMovementGenerator<Player>::ChargePathMovementGenerator(uint32, float, float, float, Movement::PointsArray, float, std::optional<float>);
template ChargePathMovementGenerator<Creature>::ChargePathMovementGenerator(uint32, float, float, float, Movement::PointsArray, float, std::optional<float>);
template void ChargePathMovementGenerator<Player>::DoInitialize(Player*);
template void ChargePathMovementGenerator<Creature>::DoInitialize(Creature*);
template void ChargePathMovementGenerator<Player>::DoReset(Player*);
template void ChargePathMovementGenerator<Creature>::DoReset(Creature*);
template bool ChargePathMovementGenerator<Player>::DoUpdate(Player*, uint32);
template bool ChargePathMovementGenerator<Creature>::DoUpdate(Creature*, uint32);
template void ChargePathMovementGenerator<Player>::DoFinalize(Player*);
template void ChargePathMovementGenerator<Creature>::DoFinalize(Creature*);
template void ChargePathMovementGenerator<Player>::MovementInform(Player*);
template void ChargePathMovementGenerator<Creature>::MovementInform(Creature*);
