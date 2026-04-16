/*
 * Copyright (C) 2019 AtieshCore <https://at-wow.org/>
 * Ported to AzerothCore: MovementGeneratorMedium API, CHARGE_MOTION_TYPE.
 */

#ifndef ACORE_CHARGEMOVEMENTGENERATOR_H
#define ACORE_CHARGEMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "MoveSplineInit.h"
#include "ObjectGuid.h"
#include <optional>

class PathGenerator;

template<class T>
class ChargeMovementGenerator : public MovementGeneratorMedium<T, ChargeMovementGenerator<T>>
{
public:
    ChargeMovementGenerator(uint32 id, float x, float y, float z, bool generatePath, float speed = 0.0f, std::optional<float> finalOrient = std::nullopt, const Movement::PointsArray* path = nullptr, ObjectGuid chargeTargetGUID = ObjectGuid::Empty);

    void DoInitialize(T*);
    void DoReset(T*);
    bool DoUpdate(T*, uint32);
    void DoFinalize(T*);

    void unitSpeedChanged() { _recalculateSpeed = true; }

    MovementGeneratorType GetMovementGeneratorType() { return CHARGE_MOTION_TYPE; }

    [[nodiscard]] uint32 GetId() const { return _movementId; }

private:
    void MovementInform(T*);

    uint32 _movementId;
    float _x, _y, _z;
    float _speed;
    bool _generatePath;
    bool _forceDestination;
    std::optional<float> _finalOrient;
    Movement::PointsArray _precomputedPath;
    ObjectGuid _chargeTargetGUID;
    bool _recalculateSpeed;
};

template<class T>
class ChargePathMovementGenerator : public MovementGeneratorMedium<T, ChargePathMovementGenerator<T>>
{
public:
    /// Precomputed path points (e.g. from PathGenerator::GetPath()).
    ChargePathMovementGenerator(uint32 id, float x, float y, float z, Movement::PointsArray pathPoints, float speed = 0.0f, std::optional<float> finalOrient = std::nullopt);

    void DoInitialize(T*);
    void DoReset(T*);
    bool DoUpdate(T*, uint32);
    void DoFinalize(T*);

    void unitSpeedChanged() { _recalculateSpeed = true; }

    MovementGeneratorType GetMovementGeneratorType() { return CHARGE_MOTION_TYPE; }

    [[nodiscard]] uint32 GetId() const { return _movementId; }

private:
    void MovementInform(T*);

    uint32 _movementId;
    float _x, _y, _z;
    Movement::PointsArray _pathPoints;
    float _speed;
    std::optional<float> _finalOrient;
    bool _recalculateSpeed;
};

#endif
