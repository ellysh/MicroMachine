#include "combat_upgrades.h"
#include "../common/unit_lists.h"

CombatUpgrades::CombatUpgrades() {
    // It's a bit tricky to set the template argument to the upgrades array based on the availableUpgrades list
    // so instead we just validate it at runtime here.
    // If new upgrades are introduced the upgrades bitset needs to be expanded.
    assert(upgrades.size() >= availableUpgrades.size());
}

sc2::UPGRADE_ID CombatUpgrades::iterator::operator*() const { return availableUpgrades.getBuildOrderItem(index).upgradeID(); }

bool CombatUpgrades::hasUpgrade(sc2::UPGRADE_ID upgrade) const {
	const auto index = availableUpgrades.getIndex(upgrade);
	if (index < 0 || index >= upgrades.size())
		return false;
    return upgrades[index];
}

void CombatUpgrades::add(sc2::UPGRADE_ID upgrade) {
	const auto index = availableUpgrades.getIndex(upgrade);
	if (index >= 0 && index < upgrades.size())
		upgrades[index] = true;
}