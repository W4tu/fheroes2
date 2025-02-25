/***************************************************************************
 *   Free Heroes of Might and Magic II: https://github.com/ihhub/fheroes2  *
 *   Copyright (C) 2020 - 2022                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <map>

#include "agg.h"
#include "battle_animation.h"
#include "battle_cell.h"
#include "bin_info.h"
#include "logging.h"
#include "monster.h"
#include "serialize.h"

namespace
{
    template <typename T>
    T getValue( const uint8_t * data, const size_t base, const size_t offset = 0 )
    {
        return fheroes2::getLEValue<T>( reinterpret_cast<const char *>( data ), base, offset );
    }
}

namespace Bin_Info
{
    class MonsterAnimCache
    {
    public:
        AnimationReference createAnimReference( int monsterID ) const;
        MonsterAnimInfo getAnimInfo( int monsterID );

    private:
        std::map<int, MonsterAnimInfo> _animMap;
    };

    const size_t CORRECT_FRM_LENGTH = 821;

    // When base unit and its upgrade use the same FRM file (e.g. Archer and Ranger)
    // We modify animation speed value to make them go faster
    const double MOVE_SPEED_UPGRADE = 0.12;
    const double SHOOT_SPEED_UPGRADE = 0.08;
    const double RANGER_SHOOT_SPEED = 0.78;

    std::map<int, AnimationReference> animRefs;
    MonsterAnimCache _infoCache;

    const char * GetFilename( int monsterId )
    {
        return fheroes2::getMonsterData( monsterId ).binFileName;
    }

    MonsterAnimInfo::MonsterAnimInfo( int monsterID, const std::vector<u8> & bytes )
        : moveSpeed( 450 )
        , shootSpeed( 0 )
        , flightSpeed( 0 )
        , troopCountOffsetLeft( 0 )
        , troopCountOffsetRight( 0 )
        , idleAnimationCount( 0 )
        , idleAnimationDelay( 0 )
    {
        if ( bytes.size() != Bin_Info::CORRECT_FRM_LENGTH ) {
            return;
        }

        const uint8_t * data = bytes.data();

        eyePosition = fheroes2::Point( getValue<int16_t>( data, 1 ), getValue<int16_t>( data, 3 ) );

        for ( size_t moveID = 0; moveID < 7; ++moveID ) {
            std::vector<int> moveOffset;

            moveOffset.reserve( 16 );

            for ( int frame = 0; frame < 16; ++frame ) {
                moveOffset.push_back( static_cast<int>( getValue<int8_t>( data, 5 + moveID * 16, frame ) ) );
            }

            frameXOffset.push_back( moveOffset );
        }

        // Idle animations data
        idleAnimationCount = data[117];
        if ( idleAnimationCount > 5u )
            idleAnimationCount = 5u; // here we need to reset our object
        for ( uint32_t i = 0; i < idleAnimationCount; ++i )
            idlePriority.push_back( getValue<float>( data, 118, i ) );

        for ( uint32_t i = 0; i < idleAnimationCount; ++i )
            unusedIdleDelays.push_back( getValue<uint32_t>( data, 138, i ) );

        idleAnimationDelay = getValue<uint32_t>( data, 158 );

        // Monster speed data
        moveSpeed = getValue<uint32_t>( data, 162 );
        shootSpeed = getValue<uint32_t>( data, 166 );
        flightSpeed = getValue<uint32_t>( data, 170 );

        // Projectile data
        for ( size_t i = 0; i < 3; ++i ) {
            projectileOffset.emplace_back( getValue<int16_t>( data, 174 + ( i * 4 ) ), getValue<int16_t>( data, 176 + ( i * 4 ) ) );
        }

        // Elves and Grand Elves have incorrect start Y position for lower shooting attack
        if ( monsterID == Monster::ELF || monsterID == Monster::GRAND_ELF ) {
            if ( projectileOffset[2].y == -1 )
                projectileOffset[2].y = -32;
        }

        uint8_t projectileCount = data[186];
        if ( projectileCount > 12u )
            projectileCount = 12u; // here we need to reset our object
        for ( uint8_t i = 0; i < projectileCount; ++i )
            projectileAngles.push_back( getValue<float>( data, 187, i ) );

        // Positional offsets for sprites & drawing
        troopCountOffsetLeft = getValue<int32_t>( data, 235 );
        troopCountOffsetRight = getValue<int32_t>( data, 239 );

        // Load animation sequences themselves
        for ( int idx = MOVE_START; idx <= SHOOT3_END; ++idx ) {
            std::vector<int> anim;
            uint8_t count = data[243 + idx];
            if ( count > 16 )
                count = 16; // here we need to reset our object
            for ( uint8_t frame = 0; frame < count; ++frame ) {
                anim.push_back( static_cast<int>( data[277 + idx * 16 + frame] ) );
            }
            animationFrames.push_back( anim );
        }

        if ( monsterID == Monster::WOLF ) { // Wolves have incorrect frame for lower attack animation
            if ( animationFrames[ATTACK3].size() == 3 && animationFrames[ATTACK3][0] == 16 ) {
                animationFrames[ATTACK3][0] = 2;
            }
            if ( animationFrames[ATTACK3_END].size() == 3 && animationFrames[ATTACK3_END][2] == 16 ) {
                animationFrames[ATTACK3_END][2] = 2;
            }
        }

        // Modify AnimInfo for upgraded monsters without own FRM file
        int speedDiff = 0;
        switch ( monsterID ) {
        case Monster::RANGER:
        case Monster::VETERAN_PIKEMAN:
        case Monster::MASTER_SWORDSMAN:
        case Monster::CHAMPION:
        case Monster::CRUSADER:
        case Monster::ORC_CHIEF:
        case Monster::OGRE_LORD:
        case Monster::WAR_TROLL:
        case Monster::BATTLE_DWARF:
        case Monster::GRAND_ELF:
        case Monster::GREATER_DRUID:
        case Monster::MINOTAUR_KING:
        case Monster::STEEL_GOLEM:
        case Monster::ARCHMAGE:
        case Monster::MUTANT_ZOMBIE:
        case Monster::ROYAL_MUMMY:
        case Monster::VAMPIRE_LORD:
        case Monster::POWER_LICH:
            speedDiff = static_cast<int>( Monster( monsterID ).GetSpeed() ) - Monster( monsterID - 1 ).GetSpeed();
            break;
        case Monster::EARTH_ELEMENT:
        case Monster::AIR_ELEMENT:
        case Monster::WATER_ELEMENT:
            speedDiff = static_cast<int>( Monster( monsterID ).GetSpeed() ) - Monster( Monster::FIRE_ELEMENT ).GetSpeed();
            break;
        default:
            break;
        }

        if ( std::abs( speedDiff ) > 0 ) {
            moveSpeed = static_cast<uint32_t>( ( 1 - MOVE_SPEED_UPGRADE * speedDiff ) * moveSpeed );
            // Ranger is special since he gets double attack on upgrade
            if ( monsterID == Monster::RANGER ) {
                shootSpeed = static_cast<uint32_t>( shootSpeed * RANGER_SHOOT_SPEED );
            }
            else {
                shootSpeed = static_cast<uint32_t>( ( 1 - SHOOT_SPEED_UPGRADE * speedDiff ) * shootSpeed );
            }
        }

        if ( frameXOffset[MOVE_STOP][0] == 0 && frameXOffset[MOVE_TILE_END][0] != 0 )
            frameXOffset[MOVE_STOP][0] = frameXOffset[MOVE_TILE_END][0];

        for ( int idx = MOVE_START; idx <= MOVE_ONE; ++idx )
            frameXOffset[idx].resize( animationFrames[idx].size(), 0 );

        if ( frameXOffset[MOVE_STOP].size() == 1 && frameXOffset[MOVE_STOP][0] == 0 ) {
            if ( frameXOffset[MOVE_TILE_END].size() == 1 && frameXOffset[MOVE_TILE_END][0] != 0 )
                frameXOffset[MOVE_STOP][0] = frameXOffset[MOVE_TILE_END][0];
            else if ( frameXOffset[MOVE_TILE_START].size() == 1 && frameXOffset[MOVE_TILE_START][0] != 0 )
                frameXOffset[MOVE_STOP][0] = 44 + frameXOffset[MOVE_TILE_START][0];
            else
                frameXOffset[MOVE_STOP][0] = frameXOffset[MOVE_MAIN].back();
        }

        if ( monsterID == Monster::IRON_GOLEM || monsterID == Monster::STEEL_GOLEM ) {
            if ( frameXOffset[MOVE_START].size() == 4 ) { // the original golem info
                frameXOffset[MOVE_START][0] = 0;
                frameXOffset[MOVE_START][1] = CELLW * 1 / 8;
                frameXOffset[MOVE_START][2] = CELLW * 2 / 8;
                frameXOffset[MOVE_START][3] = CELLW * 3 / 8;
                for ( size_t id = 0; id < frameXOffset[MOVE_MAIN].size(); ++id )
                    frameXOffset[MOVE_MAIN][id] += CELLW / 2;
            }
        }

        if ( monsterID == Monster::SWORDSMAN || monsterID == Monster::MASTER_SWORDSMAN ) {
            if ( frameXOffset[MOVE_START].size() == 2 && frameXOffset[MOVE_STOP].size() == 1 ) { // the original swordsman info
                frameXOffset[MOVE_START][0] = 0;
                frameXOffset[MOVE_START][1] = CELLW * 1 / 8;
                for ( size_t id = 0; id < frameXOffset[MOVE_MAIN].size(); ++id )
                    frameXOffset[MOVE_MAIN][id] += CELLW / 4;

                frameXOffset[MOVE_STOP][0] = CELLW;
            }
        }
    }

    MonsterAnimInfo MonsterAnimCache::getAnimInfo( int monsterID )
    {
        std::map<int, MonsterAnimInfo>::iterator mapIterator = _animMap.find( monsterID );
        if ( mapIterator != _animMap.end() ) {
            return mapIterator->second;
        }
        else {
            const MonsterAnimInfo info( monsterID, AGG::ReadChunk( Bin_Info::GetFilename( monsterID ) ) );
            if ( info.isValid() ) {
                _animMap[monsterID] = info;
                return info;
            }
            else {
                DEBUG_LOG( DBG_ENGINE, DBG_WARN, "missing BIN FRM data: " << Bin_Info::GetFilename( monsterID ) << ", index: " << monsterID );
            }
        }
        return MonsterAnimInfo();
    }

    bool MonsterAnimInfo::isValid() const
    {
        if ( animationFrames.size() != SHOOT3_END + 1 )
            return false;

        // Absolute minimal set up
        const int essentialAnimations[7] = {MOVE_MAIN, STATIC, DEATH, WINCE_UP, ATTACK1, ATTACK2, ATTACK3};

        for ( int i = 0; i < 7; ++i ) {
            if ( animationFrames.at( essentialAnimations[i] ).empty() )
                return false;
        }

        if ( idlePriority.size() != static_cast<size_t>( idleAnimationCount ) )
            return false;

        return true;
    }

    bool MonsterAnimInfo::hasAnim( int animID ) const
    {
        return animationFrames.size() == SHOOT3_END + 1 && !animationFrames.at( animID ).empty();
    }

    size_t MonsterAnimInfo::getProjectileID( const double angle ) const
    {
        const std::vector<float> & angles = projectileAngles;
        if ( angles.empty() )
            return 0;

        for ( size_t id = 0u; id < angles.size() - 1; ++id ) {
            if ( angle >= static_cast<double>( angles[id] + angles[id + 1] ) / 2.0 )
                return id;
        }
        return angles.size() - 1;
    }

    AnimationReference MonsterAnimCache::createAnimReference( int monsterID ) const
    {
        return AnimationReference( monsterID );
    }

    MonsterAnimInfo GetMonsterInfo( uint32_t monsterID )
    {
        return _infoCache.getAnimInfo( monsterID );
    }

    void InitBinInfo()
    {
        for ( int i = Monster::UNKNOWN; i < Monster::WATER_ELEMENT + 1; ++i )
            animRefs[i] = _infoCache.createAnimReference( i );
    }
}
