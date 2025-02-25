/***************************************************************************
 *   Free Heroes of Might and Magic II: https://github.com/ihhub/fheroes2  *
 *   Copyright (C) 2019 - 2022                                             *
 *                                                                         *
 *   Free Heroes2 Engine: http://sourceforge.net/projects/fheroes2         *
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include <array>
#include <map>
#include <string>

#include "agg_image.h"
#include "artifact.h"
#include "dialog.h"
#include "dialog_selectitems.h"
#include "heroes.h"
#include "icn.h"
#include "logging.h"
#include "rand.h"
#include "serialize.h"
#include "settings.h"
#include "spell.h"
#include "statusbar.h"
#include "text.h"
#include "tools.h"
#include "translations.h"
#include "ui_dialog.h"

namespace
{
    const std::map<ArtifactSetData, std::vector<uint32_t>> artifactSets
        = { { ArtifactSetData( Artifact::BATTLE_GARB, gettext_noop( "The three Anduran artifacts magically combine into one." ) ),
              { Artifact::HELMET_ANDURAN, Artifact::SWORD_ANDURAN, Artifact::BREASTPLATE_ANDURAN } } };

    std::array<uint8_t, Artifact::UNKNOWN + 1> artifactGlobalStatus = {};

    enum
    {
        ART_RNDDISABLED = 0x01,
        ART_RNDUSED = 0x02
    };
}

const char * Artifact::GetName( void ) const
{
    return _( fheroes2::getArtifactData( id ).name );
}

bool Artifact::isUltimate( void ) const
{
    switch ( id ) {
    case ULTIMATE_BOOK:
    case ULTIMATE_SWORD:
    case ULTIMATE_CLOAK:
    case ULTIMATE_WAND:
    case ULTIMATE_SHIELD:
    case ULTIMATE_STAFF:
    case ULTIMATE_CROWN:
    case GOLDEN_GOOSE:
        return true;
    default:
        break;
    }

    return false;
}

int Artifact::LoyaltyLevel( void ) const
{
    switch ( id ) {
    case MASTHEAD:
    case SPADE_NECROMANCY:
    case HEART_FIRE:
    case HEART_ICE:
        return ART_LEVEL2;

    case ARM_MARTYR:
    case HOLY_HAMMER:
    case LEGENDARY_SCEPTER:
    case STAFF_WIZARDRY:
    case SWORD_BREAKER:
    case CRYSTAL_BALL:
        return ART_LEVEL3;

    case SPELL_SCROLL:
    case BROACH_SHIELDING:
    case SWORD_ANDURAN:
    case BREASTPLATE_ANDURAN:
    case BATTLE_GARB:
    case HELMET_ANDURAN:
    case SPHERE_NEGATION:
        return ART_NORANDOM;

    default:
        break;
    }

    return ART_NONE;
}

int Artifact::Level( void ) const
{
    if ( isUltimate() ) {
        return ART_ULTIMATE;
    }

    switch ( id ) {
    case MEDAL_VALOR:
    case MEDAL_COURAGE:
    case MEDAL_HONOR:
    case MEDAL_DISTINCTION:
    case THUNDER_MACE:
    case ARMORED_GAUNTLETS:
    case DEFENDER_HELM:
    case GIANT_FLAIL:
    case RABBIT_FOOT:
    case GOLDEN_HORSESHOE:
    case GAMBLER_LUCKY_COIN:
    case FOUR_LEAF_CLOVER:
    case ENCHANTED_HOURGLASS:
    case ICE_CLOAK:
    case FIRE_CLOAK:
    case LIGHTNING_HELM:
    case SNAKE_RING:
    case HOLY_PENDANT:
    case PENDANT_FREE_WILL:
    case PENDANT_LIFE:
    case SERENITY_PENDANT:
    case SEEING_EYE_PENDANT:
    case KINETIC_PENDANT:
    case PENDANT_DEATH:
    case GOLDEN_BOW:
    case TELESCOPE:
    case STATESMAN_QUILL:
        return ART_LEVEL1;

    case CASTER_BRACELET:
    case MAGE_RING:
    case STEALTH_SHIELD:
    case POWER_AXE:
    case MINOR_SCROLL:
    case ENDLESS_PURSE_GOLD:
    case SAILORS_ASTROLABE_MOBILITY:
    case EVIL_EYE:
    case GOLD_WATCH:
    case SKULLCAP:
    case EVERCOLD_ICICLE:
    case EVERHOT_LAVA_ROCK:
    case LIGHTNING_ROD:
    case ANKH:
    case BOOK_ELEMENTS:
    case ELEMENTAL_RING:
    case POWER_RING:
    case AMMO_CART:
    case ENDLESS_CORD_WOOD:
    case ENDLESS_CART_ORE:
    case SPIKED_HELM:
    case WHITE_PEARL:
        return ART_LEVEL2;

    case ARCANE_NECKLACE:
    case WITCHES_BROACH:
    case BALLISTA:
    case DRAGON_SWORD:
    case DIVINE_BREASTPLATE:
    case MAJOR_SCROLL:
    case SUPERIOR_SCROLL:
    case FOREMOST_SCROLL:
    case ENDLESS_SACK_GOLD:
    case ENDLESS_BAG_GOLD:
    case NOMAD_BOOTS_MOBILITY:
    case TRAVELER_BOOTS_MOBILITY:
    case TRUE_COMPASS_MOBILITY:
    case WAND_NEGATION:
    case WIZARD_HAT:
    case ENDLESS_POUCH_SULFUR:
    case ENDLESS_VIAL_MERCURY:
    case ENDLESS_POUCH_GEMS:
    case ENDLESS_POUCH_CRYSTAL:
    case SPIKED_SHIELD:
    case BLACK_PEARL:
        return ART_LEVEL3;

    // no random
    case MAGIC_BOOK:
    case FIZBIN_MISFORTUNE:
    case TAX_LIEN:
    case HIDEOUS_MASK:
        return ART_NORANDOM;

    // price loyalty
    case SPELL_SCROLL:
    case ARM_MARTYR:
    case BREASTPLATE_ANDURAN:
    case BROACH_SHIELDING:
    case BATTLE_GARB:
    case CRYSTAL_BALL:
    case HELMET_ANDURAN:
    case HOLY_HAMMER:
    case LEGENDARY_SCEPTER:
    case MASTHEAD:
    case SPHERE_NEGATION:
    case STAFF_WIZARDRY:
    case SWORD_BREAKER:
    case SWORD_ANDURAN:
    case SPADE_NECROMANCY:
    case HEART_FIRE:
    case HEART_ICE:
        return Settings::Get().isCurrentMapPriceOfLoyalty() ? ART_LOYALTY | LoyaltyLevel() : ART_LOYALTY;

    default:
        break;
    }

    return ART_NONE;
}

int Artifact::getArtifactValue() const
{
    // TODO: this method should return a value of the artifact based on its bonuses and curses.
    // Right now it is based on a level of an artifact which makes some artifacts with different stats be valued as equal.
    const int level = Level();

    if ( level & ART_LEVEL1 ) {
        return 1;
    }
    else if ( level & ART_LEVEL2 ) {
        return 2;
    }
    else if ( level & ART_LEVEL3 ) {
        return 3;
    }
    else if ( level & ART_ULTIMATE ) {
        return 5;
    }

    return 0;
}

void Artifact::SetSpell( int v )
{
    if ( id != SPELL_SCROLL ) {
        // This method must be called only for Spell Scroll artifact.
        assert( 0 );
        return;
    }

    const bool adv = Rand::Get( 1 ) != 0;

    switch ( v ) {
    case Spell::RANDOM:
        ext = Spell::Rand( Rand::Get( 1, 5 ), adv ).GetID();
        break;
    case Spell::RANDOM1:
        ext = Spell::Rand( 1, adv ).GetID();
        break;
    case Spell::RANDOM2:
        ext = Spell::Rand( 2, adv ).GetID();
        break;
    case Spell::RANDOM3:
        ext = Spell::Rand( 3, adv ).GetID();
        break;
    case Spell::RANDOM4:
        ext = Spell::Rand( 4, adv ).GetID();
        break;
    case Spell::RANDOM5:
        ext = Spell::Rand( 5, adv ).GetID();
        break;
    default:
        ext = v;
        break;
    }
}

/* get rand all artifact */
int Artifact::Rand( level_t lvl )
{
    std::vector<int> v;
    v.reserve( 25 );

    // if possibly: make unique on map
    for ( int art = ULTIMATE_BOOK; art < UNKNOWN; ++art )
        if ( ( lvl & Artifact( art ).Level() ) && !( artifactGlobalStatus[art] & ART_RNDDISABLED ) && !( artifactGlobalStatus[art] & ART_RNDUSED ) )
            v.push_back( art );

    if ( v.empty() ) {
        for ( int art = ULTIMATE_BOOK; art < UNKNOWN; ++art )
            if ( ( lvl & Artifact( art ).Level() ) && !( artifactGlobalStatus[art] & ART_RNDDISABLED ) )
                v.push_back( art );
    }

    int res = !v.empty() ? Rand::Get( v ) : Artifact::UNKNOWN;
    artifactGlobalStatus[res] |= ART_RNDUSED;

    return res;
}

Artifact Artifact::FromMP2IndexSprite( u32 index )
{
    if ( 0xA2 > index )
        return Artifact( ( index - 1 ) / 2 );
    else if ( Settings::Get().isPriceOfLoyaltySupported() && 0xAB < index && 0xCE > index )
        return Artifact( ( index - 1 ) / 2 );
    else if ( 0xA3 == index )
        return Artifact( Rand( ART_LEVEL123 ) );
    else if ( 0xA4 == index )
        return Artifact( Rand( ART_ULTIMATE ) );
    else if ( 0xA7 == index )
        return Artifact( Rand( ART_LEVEL1 ) );
    else if ( 0xA9 == index )
        return Artifact( Rand( ART_LEVEL2 ) );
    else if ( 0xAB == index )
        return Rand( ART_LEVEL3 );

    DEBUG_LOG( DBG_GAME, DBG_WARN, "unknown index: " << static_cast<int>( index ) );

    return Artifact( UNKNOWN );
}

const char * Artifact::getDiscoveryDescription( const Artifact & art )
{
    return _( fheroes2::getArtifactData( art.GetID() ).discoveryEventDescription );
}

StreamBase & operator<<( StreamBase & msg, const Artifact & art )
{
    return msg << art.id << art.ext;
}

StreamBase & operator>>( StreamBase & msg, Artifact & art )
{
    return msg >> art.id >> art.ext;
}

BagArtifacts::BagArtifacts()
    : std::vector<Artifact>( HEROESMAXARTIFACT, Artifact::UNKNOWN )
{}

bool BagArtifacts::ContainSpell( const int spellId ) const
{
    assert( spellId > Spell::NONE && spellId < Spell::SPELL_COUNT );

    for ( const Artifact & artifact : *this ) {
        const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifact.GetID() ).bonuses;
        for ( const fheroes2::ArtifactBonus & bonus : bonuses ) {
            if ( bonus.type == fheroes2::ArtifactBonusType::ADD_SPELL ) {
                assert( bonus.value > Spell::NONE && bonus.value < Spell::SPELL_COUNT );
                if ( bonus.value == spellId ) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool BagArtifacts::isPresentArtifact( const Artifact & art ) const
{
    return end() != std::find( begin(), end(), art );
}

bool BagArtifacts::isArtifactBonusPresent( const fheroes2::ArtifactBonusType type ) const
{
    for ( const Artifact & artifact : *this ) {
        const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifact.GetID() ).bonuses;
        if ( std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( type ) ) != bonuses.end() ) {
            return true;
        }
    }

    return false;
}

bool BagArtifacts::isArtifactCursePresent( const fheroes2::ArtifactCurseType type ) const
{
    for ( const Artifact & artifact : *this ) {
        const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifact.GetID() ).curses;
        if ( std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( type ) ) != curses.end() ) {
            return true;
        }
    }

    return false;
}

int32_t BagArtifacts::getTotalArtifactEffectValue( const fheroes2::ArtifactBonusType bonus ) const
{
    // If this assertion blows up you're calling the method for a wrong type.
    assert( !fheroes2::isBonusMultiplied( bonus ) && !fheroes2::isBonusUnique( bonus ) );

    int32_t totalValue = 0;

    if ( fheroes2::isBonusCumulative( bonus ) ) {
        for ( const Artifact & artifact : *this ) {
            const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifact.GetID() ).bonuses;
            auto bonusIter = std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( bonus ) );
            if ( bonusIter != bonuses.end() ) {
                totalValue += bonusIter->value;
            }
        }
    }
    else {
        std::set<int> usedArtifactIds;
        for ( const Artifact & artifact : *this ) {
            const int artifactId = artifact.GetID();
            if ( !usedArtifactIds.insert( artifactId ).second ) {
                // The artifact is present in multiple copies.
                continue;
            }

            const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifactId ).bonuses;
            auto bonusIter = std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( bonus ) );
            if ( bonusIter != bonuses.end() ) {
                totalValue += bonusIter->value;
            }
        }
    }

    return totalValue;
}

int32_t BagArtifacts::getTotalArtifactEffectValue( const fheroes2::ArtifactBonusType bonus, std::string & description ) const
{
    // If this assertion blows up you're calling the method for a wrong type.
    assert( !fheroes2::isBonusMultiplied( bonus ) && !fheroes2::isBonusUnique( bonus ) );

    int32_t totalValue = 0;

    if ( fheroes2::isBonusCumulative( bonus ) ) {
        std::map<int, int> artifactValuePerId;
        for ( const Artifact & artifact : *this ) {
            const int artifactId = artifact.GetID();

            const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifactId ).bonuses;
            auto bonusIter = std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( bonus ) );
            if ( bonusIter != bonuses.end() ) {
                totalValue += bonusIter->value;
                artifactValuePerId[artifactId] += bonusIter->value;
            }
        }

        for ( const auto & artifactInfo : artifactValuePerId ) {
            description += Artifact( artifactInfo.first ).GetName();
            description += " +";

            description += std::to_string( artifactInfo.second );
            description += '\n';
        }
    }
    else {
        std::set<int> usedArtifactIds;
        for ( const Artifact & artifact : *this ) {
            const int artifactId = artifact.GetID();
            if ( !usedArtifactIds.insert( artifactId ).second ) {
                // The artifact is present in multiple copies.
                continue;
            }

            const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifactId ).bonuses;
            auto bonusIter = std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( bonus ) );
            if ( bonusIter != bonuses.end() ) {
                totalValue += bonusIter->value;

                description += artifact.GetName();
                description += " +"; // to show a positive value.

                description += std::to_string( bonusIter->value );
                description += '\n';
            }
        }
    }

    return totalValue;
}

int32_t BagArtifacts::getTotalArtifactEffectValue( const fheroes2::ArtifactCurseType curse ) const
{
    // If this assertion blows up you're calling the method for a wrong type.
    assert( !fheroes2::isCurseMultiplied( curse ) && !fheroes2::isCurseUnique( curse ) );

    int32_t totalValue = 0;

    if ( fheroes2::isCurseCumulative( curse ) ) {
        for ( const Artifact & artifact : *this ) {
            const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifact.GetID() ).curses;
            auto curseIter = std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( curse ) );
            if ( curseIter != curses.end() ) {
                totalValue += curseIter->value;
            }
        }
    }
    else {
        std::set<int> usedArtifactIds;
        for ( const Artifact & artifact : *this ) {
            const int artifactId = artifact.GetID();
            if ( !usedArtifactIds.insert( artifactId ).second ) {
                // The artifact is present in multiple copies.
                continue;
            }

            const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifactId ).curses;
            auto curseIter = std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( curse ) );
            if ( curseIter != curses.end() ) {
                totalValue += curseIter->value;
            }
        }
    }

    return totalValue;
}

int32_t BagArtifacts::getTotalArtifactEffectValue( const fheroes2::ArtifactCurseType curse, std::string & description ) const
{
    // If this assertion blows up you're calling the method for a wrong type.
    assert( !fheroes2::isCurseMultiplied( curse ) && !fheroes2::isCurseUnique( curse ) );

    int32_t totalValue = 0;

    if ( fheroes2::isCurseCumulative( curse ) ) {
        std::map<int, int> artifactValuePerId;
        for ( const Artifact & artifact : *this ) {
            const int artifactId = artifact.GetID();

            const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifactId ).curses;
            auto curseIter = std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( curse ) );
            if ( curseIter != curses.end() ) {
                totalValue += curseIter->value;
                artifactValuePerId[artifactId] += curseIter->value;
            }
        }

        for ( const auto & artifactInfo : artifactValuePerId ) {
            description += Artifact( artifactInfo.first ).GetName();
            description += " -";

            description += std::to_string( artifactInfo.second );
            description += '\n';
        }
    }
    else {
        std::set<int> usedArtifactIds;
        for ( const Artifact & artifact : *this ) {
            const int artifactId = artifact.GetID();
            if ( !usedArtifactIds.insert( artifactId ).second ) {
                // The artifact is present in multiple copies.
                continue;
            }

            const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifactId ).curses;
            auto curseIter = std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( curse ) );
            if ( curseIter != curses.end() ) {
                totalValue += curseIter->value;

                description += artifact.GetName();
                description += " -";

                description += std::to_string( curseIter->value );
                description += '\n';
            }
        }
    }

    return totalValue;
}

std::vector<int32_t> BagArtifacts::getTotalArtifactMultipliedPercent( const fheroes2::ArtifactBonusType bonus ) const
{
    if ( !fheroes2::isBonusMultiplied( bonus ) ) {
        // You are calling this method for a wrong bonus type!
        assert( 0 );
        return {};
    }

    std::vector<int32_t> values;

    std::set<int> usedArtifactIds;
    for ( const Artifact & artifact : *this ) {
        const int artifactId = artifact.GetID();
        if ( !usedArtifactIds.insert( artifactId ).second ) {
            // The artifact is present in multiple copies.
            continue;
        }

        const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifactId ).bonuses;
        auto bonusIter = std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( bonus ) );
        if ( bonusIter != bonuses.end() ) {
            values.emplace_back( bonusIter->value );
        }
    }

    return values;
}

std::vector<int32_t> BagArtifacts::getTotalArtifactMultipliedPercent( const fheroes2::ArtifactCurseType curse ) const
{
    if ( !fheroes2::isCurseMultiplied( curse ) ) {
        // You are calling this method for a wrong curse type!
        assert( 0 );
        return {};
    }

    std::vector<int32_t> values;

    std::set<int> usedArtifactIds;
    for ( const Artifact & artifact : *this ) {
        const int artifactId = artifact.GetID();
        if ( !usedArtifactIds.insert( artifactId ).second ) {
            // The artifact is present in multiple copies.
            continue;
        }

        const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifactId ).curses;
        auto curseIter = std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( curse ) );
        if ( curseIter != curses.end() ) {
            values.emplace_back( curseIter->value );
        }
    }

    return values;
}

Artifact BagArtifacts::getFirstArtifactWithBonus( const fheroes2::ArtifactBonusType bonus ) const
{
    for ( const Artifact & artifact : *this ) {
        const std::vector<fheroes2::ArtifactBonus> & bonuses = fheroes2::getArtifactData( artifact.GetID() ).bonuses;
        auto bonusIter = std::find( bonuses.begin(), bonuses.end(), fheroes2::ArtifactBonus( bonus ) );
        if ( bonusIter != bonuses.end() ) {
            return artifact;
        }
    }

    return { Artifact::UNKNOWN };
}

Artifact BagArtifacts::getFirstArtifactWithCurse( const fheroes2::ArtifactCurseType curse ) const
{
    for ( const Artifact & artifact : *this ) {
        const std::vector<fheroes2::ArtifactCurse> & curses = fheroes2::getArtifactData( artifact.GetID() ).curses;
        auto curseIter = std::find( curses.begin(), curses.end(), fheroes2::ArtifactCurse( curse ) );
        if ( curseIter != curses.end() ) {
            return artifact;
        }
    }

    return { Artifact::UNKNOWN };
}

bool BagArtifacts::PushArtifact( const Artifact & art )
{
    if ( !art.isValid() ) {
        // Why an invalid artifact is being pushed?
        assert( 0 );
        return false;
    }

    if ( art.GetID() == Artifact::MAGIC_BOOK && isPresentArtifact( art ) ) {
        // Why do you push another magic book into the bag?
        assert( 0 );
        return false;
    }

    iterator it = std::find( begin(), end(), Artifact( Artifact::UNKNOWN ) );
    if ( it == end() )
        return false;

    *it = art;

    // Always put Magic Book at first place.
    if ( art.GetID() == Artifact::MAGIC_BOOK ) {
        std::swap( *it, front() );
    }

    return true;
}

void BagArtifacts::RemoveArtifact( const Artifact & art )
{
    iterator it = std::find( begin(), end(), art );
    if ( it != end() )
        ( *it ).Reset();
}

bool BagArtifacts::isFull( void ) const
{
    return end() == std::find( begin(), end(), Artifact( Artifact::UNKNOWN ) );
}

u32 BagArtifacts::CountArtifacts( void ) const
{
    // no way that we have more than 4 billion artifacts so static_cast is totally valid
    return static_cast<uint32_t>( std::count_if( begin(), end(), []( const Artifact & art ) { return art.isValid(); } ) );
}

int BagArtifacts::getArtifactValue() const
{
    int result = 0;
    for ( const Artifact & art : *this ) {
        if ( art.isValid() )
            result += art.getArtifactValue();
    }

    return result;
}

void BagArtifacts::exchangeArtifacts( BagArtifacts & giftBag )
{
    std::vector<Artifact> combined;
    for ( auto it = begin(); it != end(); ++it ) {
        if ( it->isValid() && it->GetID() != Artifact::MAGIC_BOOK ) {
            combined.push_back( *it );
            it->Reset();
        }
    }

    for ( auto it = giftBag.begin(); it != giftBag.end(); ++it ) {
        if ( it->isValid() && it->GetID() != Artifact::MAGIC_BOOK ) {
            combined.push_back( *it );
            it->Reset();
        }
    }

    // better artifacts at the end
    std::sort( combined.begin(), combined.end(), []( const Artifact & left, const Artifact & right ) { return left.getArtifactValue() < right.getArtifactValue(); } );

    // reset and clear all current artifacts, put back the best
    while ( !combined.empty() && PushArtifact( combined.back() ) ) {
        combined.pop_back();
    }

    while ( !combined.empty() && giftBag.PushArtifact( combined.back() ) ) {
        combined.pop_back();
    }
}

bool BagArtifacts::ContainUltimateArtifact( void ) const
{
    return std::any_of( begin(), end(), []( const Artifact & art ) { return art.isUltimate(); } );
}

std::string BagArtifacts::String( void ) const
{
    std::string output;

    for ( const_iterator it = begin(); it != end(); ++it ) {
        output += it->GetName();
        output += ", ";
    }

    return output;
}

u32 BagArtifacts::Count( const Artifact & art ) const
{
    return static_cast<uint32_t>( std::count( begin(), end(), art ) ); // no way that we have more than 4 billion artifacts
}

u32 GoldInsteadArtifact( const MP2::MapObjectType objectType )
{
    switch ( objectType ) {
    case MP2::OBJ_SKELETON:
    case MP2::OBJ_TREASURECHEST:
    case MP2::OBJ_SHIPWRECKSURVIVOR:
        return 1000;
    case MP2::OBJ_WATERCHEST:
        return 1500;
    case MP2::OBJ_GRAVEYARD:
        return 2000;
    case MP2::OBJ_SHIPWRECK:
        return 5000;
    default:
        break;
    }
    return 0;
}

void fheroes2::ResetArtifactStats()
{
    std::fill( artifactGlobalStatus.begin(), artifactGlobalStatus.end(), static_cast<uint8_t>( 0 ) );
}

void fheroes2::ExcludeArtifactFromRandom( const int artifactID )
{
    const size_t id = static_cast<size_t>( artifactID );

    assert( id < artifactGlobalStatus.size() );

    artifactGlobalStatus[id] |= ART_RNDDISABLED;
}

ArtifactsBar::ArtifactsBar( const Heroes * hero, const bool mini, const bool ro, const bool change, const bool allowOpeningMagicBook, StatusBar * bar )
    : _hero( hero )
    , use_mini_sprite( mini )
    , read_only( ro )
    , can_change( change )
    , _allowOpeningMagicBook( allowOpeningMagicBook )
    , _statusBar( bar )
{
    if ( use_mini_sprite ) {
        const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::HSICONS, 0 );
        const fheroes2::Rect rt( 26, 21, 32, 32 );

        backsf.resize( rt.width + 2, rt.height + 2 );
        backsf.reset();

        fheroes2::DrawBorder( backsf, fheroes2::GetColorId( 0xD0, 0xC0, 0x48 ) );
        fheroes2::Blit( sprite, rt.x, rt.y, backsf, 1, 1, rt.width, rt.height );

        SetItemSize( backsf.width(), backsf.height() );

        spcursor.resize( backsf.width(), backsf.height() );
        spcursor.reset();
        fheroes2::DrawBorder( spcursor, 214 );
    }
    else {
        const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::ARTIFACT, 0 );
        SetItemSize( sprite.width(), sprite.height() );

        spcursor.resize( 70, 70 );
        spcursor.reset();
        fheroes2::DrawRect( spcursor, { 0, 0, 70, 70 }, 190 );
        fheroes2::DrawRect( spcursor, { 1, 1, 68, 68 }, 180 );
        fheroes2::DrawRect( spcursor, { 2, 2, 66, 66 }, 190 );
    }
}

void ArtifactsBar::ResetSelected( void )
{
    spcursor.hide();
    Interface::ItemsActionBar<Artifact>::ResetSelected();
}

void ArtifactsBar::Redraw( fheroes2::Image & dstsf )
{
    spcursor.hide();
    Interface::ItemsActionBar<Artifact>::Redraw( dstsf );
}

void ArtifactsBar::RedrawBackground( const fheroes2::Rect & pos, fheroes2::Image & dstsf )
{
    if ( use_mini_sprite )
        fheroes2::Blit( backsf, dstsf, pos.x, pos.y );
    else
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::ARTIFACT, 0 ), dstsf, pos.x, pos.y );
}

void ArtifactsBar::RedrawItem( Artifact & art, const fheroes2::Rect & pos, bool selected, fheroes2::Image & dstsf )
{
    if ( art.isValid() ) {
        if ( use_mini_sprite ) {
            const fheroes2::Sprite & artifactSprite = fheroes2::AGG::GetICN( ICN::ARTFX, art.IndexSprite32() );
            fheroes2::Fill( dstsf, pos.x + 1, pos.y + 1, artifactSprite.width(), artifactSprite.height(), 0 );
            fheroes2::Blit( artifactSprite, dstsf, pos.x + 1, pos.y + 1 );
        }
        else {
            const fheroes2::Sprite & artifactSprite = fheroes2::AGG::GetICN( ICN::ARTIFACT, art.IndexSprite64() );
            fheroes2::Fill( dstsf, pos.x, pos.y, artifactSprite.width(), artifactSprite.height(), 0 );
            fheroes2::Blit( artifactSprite, dstsf, pos.x, pos.y );
        }

        if ( selected ) {
            if ( use_mini_sprite )
                spcursor.setPosition( pos.x, pos.y );
            else
                spcursor.setPosition( pos.x - 3, pos.y - 3 );

            spcursor.show();
        }
    }
}

bool ArtifactsBar::ActionBarLeftMouseSingleClick( Artifact & art )
{
    if ( isMagicBook( art ) ) {
        const bool isMbSelected = ( !isSelected() || isMagicBook( *GetSelectedItem() ) );
        if ( isMbSelected ) {
            if ( can_change ) {
                const_cast<Heroes *>( _hero )->EditSpellBook();
            }
            else if ( _allowOpeningMagicBook ) {
                if ( _statusBar != nullptr ) {
                    std::function<void( const std::string & )> statusCallback = [this]( const std::string & status ) { _statusBar->ShowMessage( status ); };
                    _hero->OpenSpellBook( SpellBook::Filter::ALL, false, &statusCallback );
                }
                else {
                    _hero->OpenSpellBook( SpellBook::Filter::ALL, false, nullptr );
                }
            }
            else {
                messageMagicBookAbortTrading();
            }
        }

        return false;
    }

    if ( isSelected() ) {
        if ( !read_only ) {
            std::swap( art, *GetSelectedItem() );
        }
        return false;
    }

    if ( art.isValid() ) {
        if ( !read_only ) {
            spcursor.hide();
        }
    }
    else {
        if ( can_change )
            art = Dialog::SelectArtifact();

        return false;
    }

    return true;
}

bool ArtifactsBar::ActionBarLeftMouseDoubleClick( Artifact & art )
{
    if ( art.isValid() ) {
        fheroes2::ArtifactDialogElement( art ).showPopup( Dialog::OK );
    }

    ResetSelected();

    return true;
}

bool ArtifactsBar::ActionBarRightMouseHold( Artifact & art )
{
    ResetSelected();

    if ( art.isValid() ) {
        if ( can_change ) {
            art.Reset();
        }
        else {
            fheroes2::ArtifactDialogElement( art ).showPopup( Dialog::ZERO );
        }
    }

    return true;
}

bool ArtifactsBar::ActionBarLeftMouseSingleClick( Artifact & art1, Artifact & art2 )
{
    if ( !isMagicBook( art1 ) && !isMagicBook( art2 ) ) {
        std::swap( art1, art2 );
    }
    else {
        messageMagicBookAbortTrading();
    }

    return false;
}

bool ArtifactsBar::ActionBarCursor( Artifact & art )
{
    if ( isSelected() ) {
        const Artifact * art2 = GetSelectedItem();

        if ( &art == art2 ) {
            if ( isMagicBook( art ) )
                msg = _( "View Spells" );
            else {
                msg = _( "View %{name} Info" );
                StringReplace( msg, "%{name}", art.GetName() );
            }
        }
        else if ( !art.isValid() ) {
            if ( !read_only ) {
                msg = _( "Move %{name}" );
                StringReplace( msg, "%{name}", art2->GetName() );
            }
        }
        else if ( !read_only ) {
            if ( isMagicBook( art ) ) {
                msg = _( "Cannot move the Spellbook" );
            }
            else {
                msg = _( "Exchange %{name2} with %{name}" );
                StringReplace( msg, "%{name}", art.GetName() );
                StringReplace( msg, "%{name2}", art2->GetName() );
            }
        }
    }
    else if ( art.isValid() ) {
        if ( isMagicBook( art ) ) {
            msg = _( "View Spells" );
        }
        else {
            msg = _( "Select %{name}" );
            StringReplace( msg, "%{name}", art.GetName() );
        }
    }

    return false;
}

bool ArtifactsBar::ActionBarCursor( Artifact & art1, Artifact & art2 /* selected */ )
{
    if ( isMagicBook( art2 ) || isMagicBook( art1 ) )
        msg = _( "Cannot move the Spellbook" );
    else if ( art1.isValid() ) {
        msg = _( "Exchange %{name2} with %{name}" );
        StringReplace( msg, "%{name}", art1.GetName() );
        StringReplace( msg, "%{name2}", art2.GetName() );
    }
    else {
        msg = _( "Move %{name}" );
        StringReplace( msg, "%{name}", art2.GetName() );
    }

    return false;
}

bool ArtifactsBar::QueueEventProcessing( std::string * str )
{
    msg.clear();
    bool res = Interface::ItemsActionBar<Artifact>::QueueEventProcessing();
    if ( str )
        *str = msg;
    return res;
}

bool ArtifactsBar::QueueEventProcessing( ArtifactsBar & bar, std::string * str )
{
    msg.clear();
    bool res = Interface::ItemsActionBar<Artifact>::QueueEventProcessing( bar );
    if ( str )
        *str = msg;
    return res;
}

bool ArtifactsBar::isMagicBook( const Artifact & artifact )
{
    return artifact.GetID() == Artifact::MAGIC_BOOK;
}

void ArtifactsBar::messageMagicBookAbortTrading() const
{
    Dialog::Message( "", _( "This item can't be traded." ), Font::BIG, Dialog::OK );
}

ArtifactSetData::ArtifactSetData( const uint32_t artifactID, const std::string & assembleMessage )
    : _assembledArtifactID( artifactID )
    , _assembleMessage( assembleMessage )
{}

std::set<ArtifactSetData> BagArtifacts::assembleArtifactSetIfPossible()
{
    std::set<ArtifactSetData> assembledArtifactSets;

    for ( const auto & setData : artifactSets ) {
        bool foundAllArtifacts = true;
        while ( foundAllArtifacts ) {
            for ( const int artifactId : setData.second ) {
                if ( std::find( begin(), end(), Artifact( artifactId ) ) == end() ) {
                    foundAllArtifacts = false;
                    break;
                }
            }

            if ( !foundAllArtifacts )
                break;

            // At this point, we have confirmed that all the artifact parts are present
            // so remove the parts and then add the assembled artifact to BagArtifacts
            for ( const int artifactId : setData.second )
                RemoveArtifact( artifactId );

            assembledArtifactSets.insert( setData.first );
            PushArtifact( setData.first._assembledArtifactID );
        }
    }

    return assembledArtifactSets;
}

bool ArtifactSetData::operator<( const ArtifactSetData & other ) const
{
    return _assembledArtifactID < other._assembledArtifactID;
}
