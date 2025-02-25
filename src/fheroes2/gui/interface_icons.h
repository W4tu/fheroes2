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

#ifndef H2INTERFACE_ICONS_H
#define H2INTERFACE_ICONS_H

#include "interface_border.h"
#include "interface_list.h"

#include <cassert>

enum icons_t
{
    ICON_HEROES = 0x01,
    ICON_CASTLES = 0x02,
    ICON_ANY = ICON_HEROES | ICON_CASTLES
};

namespace Interface
{
    class Basic;

    using HEROES = Heroes *;
    using CASTLE = Castle *;

    class IconsBar
    {
    public:
        IconsBar( const int32_t count, const fheroes2::Image & sf )
            : marker( sf )
            , iconsCount( count )
            , show( true )
        {
            assert( count >= 0 );
        }

        void SetShow( bool f )
        {
            show = f;
        }

        void redrawBackground( fheroes2::Image & output, const fheroes2::Point & offset, const int32_t validItemCount ) const;

        void SetIconsCount( const int32_t c )
        {
            iconsCount = c;
        }

        int32_t getIconCount() const
        {
            return iconsCount;
        }

        static int32_t GetItemWidth();
        static int32_t GetItemHeight();
        static bool IsVisible( void );

    protected:
        const fheroes2::Image & marker;
        int32_t iconsCount;
        bool show;
    };

    void RedrawHeroesIcon( const Heroes &, s32, s32 );
    void RedrawCastleIcon( const Castle &, s32, s32 );

    class HeroesIcons : public Interface::ListBox<HEROES>, public IconsBar
    {
    public:
        HeroesIcons( const int32_t count, const fheroes2::Image & sf )
            : IconsBar( count, sf )
        {}

        void SetPos( s32, s32 );
        void SetShow( bool );

    protected:
        using Interface::ListBox<HEROES>::ActionListDoubleClick;
        using Interface::ListBox<HEROES>::ActionListSingleClick;
        using Interface::ListBox<HEROES>::ActionListPressRight;

        void ActionCurrentUp( void ) override;
        void ActionCurrentDn( void ) override;
        void ActionListDoubleClick( HEROES & ) override;
        void ActionListSingleClick( HEROES & ) override;
        void ActionListPressRight( HEROES & ) override;
        void RedrawItem( const HEROES &, s32 ox, s32 oy, bool current ) override;
        void RedrawBackground( const fheroes2::Point & ) override;

    private:
        fheroes2::Point _topLeftCorner;
    };

    class CastleIcons : public Interface::ListBox<CASTLE>, public IconsBar
    {
    public:
        CastleIcons( const int32_t count, const fheroes2::Image & sf )
            : IconsBar( count, sf )
        {}

        void SetPos( s32, s32 );
        void SetShow( bool );

    protected:
        using Interface::ListBox<CASTLE>::ActionListDoubleClick;
        using Interface::ListBox<CASTLE>::ActionListSingleClick;
        using Interface::ListBox<CASTLE>::ActionListPressRight;

        void ActionCurrentUp( void ) override;
        void ActionCurrentDn( void ) override;
        void ActionListDoubleClick( CASTLE & ) override;
        void ActionListSingleClick( CASTLE & ) override;
        void ActionListPressRight( CASTLE & ) override;
        void RedrawItem( const CASTLE &, s32 ox, s32 oy, bool current ) override;
        void RedrawBackground( const fheroes2::Point & ) override;

    private:
        fheroes2::Point _topLeftCorner;
    };

    class IconsPanel : public BorderWindow
    {
    public:
        explicit IconsPanel( Basic & basic );

        void SetPos( s32, s32 ) override;
        void SavePosition( void ) override;
        void SetRedraw( void ) const;
        void SetRedraw( const icons_t type ) const;

        void Redraw( void );
        void QueueEventProcessing( void );

        void Select( Heroes * const );
        void Select( Castle * const );

        bool IsSelected( const icons_t type ) const;
        void ResetIcons( const icons_t type );
        void HideIcons( const icons_t type );
        void ShowIcons( const icons_t type );
        void RedrawIcons( const icons_t type );
        void SetCurrentVisible( void );

    private:
        Basic & interface;

        fheroes2::Image sfMarker;

        CastleIcons castleIcons;
        HeroesIcons heroesIcons;
    };
}

#endif
