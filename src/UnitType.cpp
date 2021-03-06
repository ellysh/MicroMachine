#include "UnitType.h"
#include "CCBot.h"

UnitType::UnitType()
    : m_bot(nullptr)
    , m_type(0)
{

}

UnitType::UnitType(const sc2::UnitTypeID & type, CCBot & bot)
    : m_bot(&bot)
    , m_type(type)
{
    
}

sc2::UnitTypeID UnitType::getAPIUnitType() const
{
    return m_type;
}

bool UnitType::is(const sc2::UnitTypeID & type) const
{
    return m_type == type;
}

bool UnitType::operator < (const UnitType & rhs) const
{
    return m_type < rhs.m_type;
}

bool UnitType::operator == (const UnitType & rhs) const
{
    return m_type == rhs.m_type;
}

bool UnitType::isValid() const
{
    return m_type != 0;
}

std::string UnitType::getName() const
{
#ifdef SC2API
    return sc2::UnitTypeToName(m_type);
#else
    return m_type.getName();
#endif
}

CCRace UnitType::getRace() const
{
#ifdef SC2API
    return m_bot->Observation()->GetUnitTypeData()[m_type].race;
#else
    return m_type.getRace();
#endif
}

bool UnitType::isCombatUnit() const
{
    if (isWorker()) { return false; }
	if (isEgg() || isLarva()) { return false; }
	if (isBuilding())
	{
		const auto unitType = getAPIUnitType();
		if (unitType == sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING && m_bot->Strategy().getStartingStrategy() == PROXY_CYCLONES) { return true; }
		if (unitType == sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY || unitType == sc2::UNIT_TYPEID::ZERG_NYDUSCANAL) { return true; }
		if (isAttackingBuilding()) { return true; }
		return false;
	}
    return true;
}

bool UnitType::isSupplyProvider() const
{
	bool isProvider = supplyProvided() < 0 && !isResourceDepot();
	if (!isProvider)
	{
		switch (m_type.ToType())
		{
			case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT: return true;
			case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED: return true;
			case sc2::UNIT_TYPEID::PROTOSS_PYLON: return true;
			case sc2::UNIT_TYPEID::ZERG_OVERLORD: return true;
			case sc2::UNIT_TYPEID::ZERG_OVERSEER: return true;
			default:
				return false;
		}
	}
    return true;
}

bool UnitType::isResourceDepot() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::ZERG_HATCHERY                : return true;
        case sc2::UNIT_TYPEID::ZERG_LAIR                    : return true;
        case sc2::UNIT_TYPEID::ZERG_HIVE                    : return true;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER         : return true;
		case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING	: return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND        : return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING  : return true;
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS     : return true;
        case sc2::UNIT_TYPEID::PROTOSS_NEXUS                : return true;
        default: return false;
    }
#else
    return m_type.isResourceDepot();
#endif
}

bool UnitType::isRefinery(sc2::UnitTypeID type)
{
	switch (type.ToType())
	{
	case sc2::UNIT_TYPEID::TERRAN_REFINERY:
	case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
	case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:
	case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
	case sc2::UNIT_TYPEID::ZERG_EXTRACTOR:
	case sc2::UNIT_TYPEID::ZERG_EXTRACTORRICH:
		return true;
	default:
		return false;
	}
}

bool UnitType::isRefinery() const
{
	return isRefinery(m_type);
}

sc2::UNIT_TYPEID UnitType::getEnemyRefineryType(sc2::Race enemyRace)
{
	switch (enemyRace)
	{
	case sc2::Terran: return sc2::UNIT_TYPEID::TERRAN_REFINERY;
	case sc2::Protoss: return sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR;
	case sc2::Zerg: return sc2::UNIT_TYPEID::ZERG_EXTRACTOR;
	default: return sc2::UNIT_TYPEID::INVALID;
	}
}

bool UnitType::isTargetable(sc2::UnitTypeID unitTypeId)
{
	switch(unitTypeId.ToType())
	{
	case sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT:
	case sc2::UNIT_TYPEID::TERRAN_KD8CHARGE:
	case sc2::UNIT_TYPEID::PROTOSS_DISRUPTORPHASED:
		return false;
	default:
		return true;
	}
}

bool UnitType::isDetector(sc2::UnitTypeID unitTypeId)
{
	switch (unitTypeId.ToType())
	{
		case sc2::UNIT_TYPEID::PROTOSS_OBSERVER:
		case sc2::UNIT_TYPEID::PROTOSS_OBSERVERSIEGEMODE:
		case sc2::UNIT_TYPEID::ZERG_OVERSEER:
		case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
		case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER:
		case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON:
		case sc2::UNIT_TYPEID::TERRAN_RAVEN:
			return true;
		default:
			return false;
	}
}

bool UnitType::isDetector() const
{
	return isDetector(m_type);
}

bool UnitType::isGeyser() const
{
#ifdef SC2API
	sc2::UnitTypeData unitTypeData(m_bot->Observation()->GetUnitTypeData()[m_type]);
	if (unitTypeData.has_vespene)
		return true;
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER:
		case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER:
		case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER:
			std::cout << "Unit type " << m_type << " has no vespene gas but is a vespene geyser" << std::endl;
			return true;
        default: return false;
    }
#else
    return m_type == BWAPI::UnitTypes::Resource_Vespene_Geyser;
#endif
}

bool UnitType::isMineral() const
{
	sc2::UnitTypeData unitTypeData(m_bot->Observation()->GetUnitTypeData()[m_type]);
	if (unitTypeData.has_minerals)
		return true;
    switch (m_type.ToType())
    {
		//Standard
		case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD:
		case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD:
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750:
		case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
		case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
		case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD:
		case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750:
		//Rich
		case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD:
		case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD:
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750:
			std::cout << "Unit type " << m_type << " has no mineral but is a mineral field" << std::endl;
			return true;
		default:
			return false;
    }
}

bool UnitType::isMineralWallPatch() const
{
	return m_type.ToType() == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD450 || (int)m_type.ToType() == 1982;//1982 is an unknown mineral field type used for walls
}

bool UnitType::isWorker() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::TERRAN_SCV           : return true;
		case sc2::UNIT_TYPEID::TERRAN_MULE			: return true;
        case sc2::UNIT_TYPEID::PROTOSS_PROBE        : return true;
        case sc2::UNIT_TYPEID::ZERG_DRONE           : return true;
        case sc2::UNIT_TYPEID::ZERG_DRONEBURROWED   : return true;
        default: return false;
    }
#else
    return m_type.isWorker();
#endif
}

bool UnitType::isMule() const
{
	return m_type.ToType() == sc2::UNIT_TYPEID::TERRAN_MULE;
}

bool UnitType::isCreepTumor() const
{
	return m_type.ToType() == sc2::UNIT_TYPEID::ZERG_CREEPTUMOR || m_type.ToType() == sc2::UNIT_TYPEID::ZERG_CREEPTUMORQUEEN || m_type.ToType() == sc2::UNIT_TYPEID::ZERG_CREEPTUMORBURROWED;
}

CCPositionType UnitType::getAttackRange() const
{
#ifdef SC2API
    return Util::GetMaxAttackRange(m_bot->Observation()->GetUnitTypeData()[m_type], *m_bot);
#else
    // TODO: this is ground weapon range right now
    return m_type.groundWeapon().maxRange();
#endif
}

float UnitType::radius() const
{
#ifdef SC2API
	if (isMineral()) { return 1; }//Not valid since its an oval.
	if (isGeyser()) { return 1.8125f; }//Same as a Barrack
	if (isAddon()) { return 1.25f; }//Same as supply depot
	else { return m_bot->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius; }
#else
	return m_type.tileWidth() / 2;
#endif
}

int UnitType::tileWidth() const
{
#ifdef SC2API
    if (isMineral()) { return 2; }
    if (isGeyser()) { return 3; }
	if (isAddon()) { return 2; }
    else { return (int)(2 * m_bot->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius); }
#else
    return m_type.tileWidth();
#endif
}

int UnitType::tileHeight() const
{
#ifdef SC2API
    if (isMineral()) { return 1; }
    if (isGeyser()) { return 3; }
	if (isAddon()) { return 2; }
    else { return (int)(2 * m_bot->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius); }
#else
    return m_type.tileHeight();
#endif
}

bool UnitType::isAddon() const
{
#ifdef SC2API
    return m_bot->Data(*this).isAddon;
#else
    return m_type.isAddon();
#endif
}

bool UnitType::isBuilding() const
{
#ifdef SC2API
    return m_bot->Data(*this).isBuilding;
#else
    return m_type.isBuilding();
#endif
}

int UnitType::supplyProvided() const
{
#ifdef SC2API
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].food_provided;
#else
    return m_type.supplyProvided();
#endif
}

int UnitType::supplyRequired() const
{
#ifdef SC2API
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].food_required;
#else
    return m_type.supplyRequired();
#endif
}

int UnitType::mineralPrice() const
{
#ifdef SC2API
	BOT_ASSERT(m_type != 0, "Invalid type id");
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].mineral_cost;
#else
    return m_type.mineralPrice();
#endif
}

int UnitType::gasPrice() const
{
#ifdef SC2API
	BOT_ASSERT(m_type != 0, "Invalid type id");
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].vespene_cost;
#else
    return m_type.gasPrice();
#endif
}

UnitType UnitType::GetUnitTypeFromName(const std::string & name, CCBot & bot)
{
#ifdef SC2API
	const sc2::UnitTypes& unitTypes = bot.Observation()->GetUnitTypeData();
    for (const sc2::UnitTypeData & data : unitTypes)
    {
        if (name == data.name)
        {
            return UnitType(data.unit_type_id, bot);
        }
    }
#else

#endif

    return UnitType();
}

bool UnitType::isOverlord() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_OVERLORD || m_type == sc2::UNIT_TYPEID::ZERG_OVERLORDCOCOON || m_type == sc2::UNIT_TYPEID::ZERG_OVERSEER || m_type == sc2::UNIT_TYPEID::ZERG_OVERLORDTRANSPORT;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Overlord;
#endif
}

bool UnitType::isLarva() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_LARVA;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Larva;
#endif
}

bool UnitType::isEgg() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_EGG;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Egg;
#endif
}

bool UnitType::isQueen() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_QUEEN;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Queen;
#endif
}

bool UnitType::isTank() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::TERRAN_SIEGETANK || m_type == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED;
#else
    return m_type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || m_type == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode;
#endif
}

bool UnitType::isMorphedBuilding() const
{
#ifdef SC2API
    
    switch (m_type.ToType())
    {
        case sc2::UNIT_TYPEID::ZERG_LAIR:                   { return true;  }
        case sc2::UNIT_TYPEID::ZERG_HIVE:                   { return true;  }
        case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:           { return true;  }
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:    { return true;  }
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:       { return true;  }
        default:                                            { return false; }                                                            
    }
#else
    return  m_type == BWAPI::UnitTypes::Zerg_Sunken_Colony ||
            m_type == BWAPI::UnitTypes::Zerg_Spore_Colony ||
            m_type == BWAPI::UnitTypes::Zerg_Lair ||
            m_type == BWAPI::UnitTypes::Zerg_Hive ||
            m_type == BWAPI::UnitTypes::Zerg_Greater_Spire;
#endif
}

bool UnitType::isAttackingBuilding() const
{
    return isBuilding() && getAttackRange() > 0.f;
}

bool UnitType::isRepairable() const
{
	switch ((sc2::UNIT_TYPEID)m_type)
	{
		case sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING:
		case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
		case sc2::UNIT_TYPEID::TERRAN_BUNKER:
		case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
		case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
		case sc2::UNIT_TYPEID::TERRAN_CYCLONE:
		case sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING:
		case sc2::UNIT_TYPEID::TERRAN_HELLION:
		case sc2::UNIT_TYPEID::TERRAN_HELLIONTANK:
		case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
		case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
		case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
		case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
		case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
		case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
		case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
		case sc2::UNIT_TYPEID::TERRAN_RAVEN:
		case sc2::UNIT_TYPEID::TERRAN_REFINERY:
		case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
		case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
		case sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING:
		case sc2::UNIT_TYPEID::TERRAN_THOR:
		case sc2::UNIT_TYPEID::TERRAN_VIKINGASSAULT:
		case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
		case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE:
		case sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:
		case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER:
			return true;
		default:
			return false;
	}
}