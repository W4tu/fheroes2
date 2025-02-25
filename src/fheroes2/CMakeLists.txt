###########################################################################
#   Free Heroes of Might and Magic II: https://github.com/ihhub/fheroes2  #
#   Copyright (C) 2022                                                    #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program; if not, write to the                         #
#   Free Software Foundation, Inc.,                                       #
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
###########################################################################

file(GLOB_RECURSE FHEROES2_SOURCES CONFIGURE_DEPENDS *.cpp)

if(MINGW)
	set(MINGW_LIBRARIES mingw32 winmm)
endif(MINGW)

if(MACOS_APP_BUNDLE)
	set(fheroes2Icon "${CMAKE_CURRENT_SOURCE_DIR}/../resources/fheroes2.icns")
	set_source_files_properties(${fheroes2Icon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	add_executable(fheroes2 MACOSX_BUNDLE ${fheroes2Icon} ${FHEROES2_SOURCES} ${TRANSLATION_DATA})

	target_compile_definitions(fheroes2 PRIVATE
		$<$<BOOL:${MACOS_APP_BUNDLE}>:MACOS_APP_BUNDLE>
		)

	target_link_libraries(fheroes2 "-framework CoreFoundation")

	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_EXECUTABLE_NAME "${MACOSX_BUNDLE_BUNDLE_NAME}")
	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../resources/Info.plist.in")
	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_ICON_FILE "fheroes2.icns")
	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "com.fheroes2.${CMAKE_PROJECT_NAME}")
	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${CMAKE_PROJECT_VERSION}")
	set_target_properties(fheroes2 PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${CMAKE_PROJECT_VERSION}")
	set_target_properties(fheroes2 PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
else(MACOS_APP_BUNDLE)
	cmake_path(ABSOLUTE_PATH FHEROES2_DATA
		BASE_DIRECTORY ${CMAKE_INSTALL_PREFIX}
		NORMALIZE
		OUTPUT_VARIABLE FHEROES2_DATA_ABSOLUTE
		)

	add_executable(fheroes2 ${FHEROES2_SOURCES})

	target_compile_definitions(fheroes2 PRIVATE
		FHEROES2_DATA=${FHEROES2_DATA_ABSOLUTE}
		)

	install(TARGETS fheroes2 DESTINATION ${CMAKE_INSTALL_BINDIR})
endif(MACOS_APP_BUNDLE)

target_include_directories(fheroes2 PRIVATE
	agg
	ai
	army
	battle
	campaign
	castle
	dialog
	game
	gui
	h2d
	heroes
	image
	kingdom
	maps
	monster
	objects
	pocketpc
	resource
	spell
	system
	world
	)

target_link_libraries(fheroes2
	${MINGW_LIBRARIES}  # Be sure define it first. Beware WinMain@16 error!!
	${SDL_MIXER_LIBRARIES}
	engine
	Threads::Threads
	ZLIB::ZLIB
	)
