#pragma once

#include "Common.h"
#include "SquadOrder.h"
#include "Unit.h"
#include "FocusFireFiniteStateMachine.h"
#include "KitingFiniteStateMachine.h"

class CCBot;
class AlphaBetaUnit;

class MicroManager
{
protected:

    CCBot & m_bot;
    SquadOrder m_order;
    std::vector<Unit> m_units;
    std::vector<Unit> m_targets;

    virtual void executeMicro() = 0;
    void trainSubUnits(const Unit & unit) const;

public:

    MicroManager(CCBot & bot);

    const std::vector<Unit> & getUnits() const;
    void setUnits(const std::vector<Unit> & u);
    virtual void setTargets(const std::vector<Unit> & targets) = 0;
    inline void setOrder(SquadOrder order) { m_order = order; }
    void regroup(const CCPosition & regroupPosition) const;
    float getSquadPower() const;
    float getTargetsPower(float averageSquadHeight) const;
    float getUnitPower(const Unit & unit, float averageSquadHeight = -1) const;

    std::unordered_map<sc2::Tag, FocusFireFiniteStateMachine*> m_focusFireStates;
    std::unordered_map<sc2::Tag, KitingFiniteStateMachine*> m_kittingStates;
    std::unordered_map<sc2::Tag, float> m_unitHealth;
    std::unordered_map<sc2::Tag, AlphaBetaUnit *> m_units_actions;
};