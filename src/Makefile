###########################################################################
#   Free Heroes of Might and Magic II: https://github.com/ihhub/fheroes2  #
#   Copyright (C) 2021 - 2022                                             #
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

TARGET := fheroes2

#
# Common build options for fheroes2 and third-party libraries
#

# Common flags for both C and C++ compilers
CCFLAGS := -fsigned-char -pthread
# Flags for the C compiler
CFLAGS :=
# Flags for the C++ compiler
CXXFLAGS :=
# Flags for the linker
LDFLAGS := -pthread

ifdef FHEROES2_WITH_DEBUG
CCFLAGS := $(CCFLAGS) -O0 -g
else
CCFLAGS := $(CCFLAGS) -O3
endif

ifneq ($(or $(FHEROES2_WITH_ASAN),$(FHEROES2_WITH_TSAN)),)
SANITIZERS := undefined

ifdef FHEROES2_WITH_ASAN
SANITIZERS := $(SANITIZERS),address
endif
ifdef FHEROES2_WITH_TSAN
SANITIZERS := $(SANITIZERS),thread
endif

CCFLAGS := $(CCFLAGS) -fsanitize=$(SANITIZERS)
LDFLAGS := $(LDFLAGS) -fsanitize=$(SANITIZERS)
endif

#
# Platform-specific build options
#

ifndef PLATFORM
ifndef OS
OS := $(shell uname)
endif

ifeq ($(OS),Windows_NT)
PLATFORM := mingw
endif
ifeq ($(OS),FreeBSD)
PLATFORM := bsd
endif
ifeq ($(OS),Darwin)
PLATFORM := osx
endif
ifeq ($(OS),Linux)
PLATFORM := all
endif
ifeq ($(OS),Haiku)
PLATFORM := all
endif
endif

include Makefile.$(PLATFORM)

#
# Build options for third-party libraries
#

CCFLAGS_TP := $(CCFLAGS)
CFLAGS_TP := $(CFLAGS)
CXXFLAGS_TP := $(CXXFLAGS)

#
# Build options for fheroes2
#

# *FLAGS_FH2 can be passed from platform-specific Makefiles
CCFLAGS := $(CCFLAGS) $(CCFLAGS_FH2)
CFLAGS := $(CFLAGS) $(CFLAGS_FH2)
CXXFLAGS := $(CXXFLAGS) $(CXXFLAGS_FH2) -std=c++11

LIBS := $(LIBS) -lz

ifdef FHEROES2_WITH_DEBUG
CCFLAGS := $(CCFLAGS) -DWITH_DEBUG
endif
ifdef FHEROES2_WITH_IMAGE
CCFLAGS := $(CCFLAGS) -DWITH_IMAGE
endif
ifdef FHEROES2_DATA
CCFLAGS := $(CCFLAGS) -DFHEROES2_DATA="$(FHEROES2_DATA)"
endif

# TODO: Add -Wconversion -Wsign-conversion -Wextra-semi flags once we fix all the corresponding code smells
# TODO: Add -Wswitch-default -Wold-style-cast flags once SDL headers can be compiled with them
CCFLAGS := $(CCFLAGS) -pedantic -Wall -Wextra -Wcast-align -Wctor-dtor-privacy -Wdouble-promotion -Wfloat-conversion -Wfloat-equal \
           -Winit-self -Woverloaded-virtual -Wredundant-decls -Wshadow -Wundef -Wuninitialized -Wunused

ifdef FHEROES2_STRICT_COMPILATION
CCFLAGS := $(CCFLAGS) -Werror
endif

#
# SDL-related build options
#

# SDL_FLAGS can already be defined in a platform-specific Makefile
ifeq ($(origin SDL_FLAGS),undefined)
ifdef FHEROES2_WITH_SDL1
SDL_FLAGS := $(shell sdl-config --cflags)
else
SDL_FLAGS := $(shell sdl2-config --cflags)
endif

ifdef FHEROES2_WITH_IMAGE
SDL_FLAGS := $(SDL_FLAGS) $(shell libpng-config --cflags)
endif
endif

# SDL_LIBS can already be defined in a platform-specific Makefile
ifeq ($(origin SDL_LIBS),undefined)
ifdef FHEROES2_WITH_SDL1
SDL_LIBS := $(shell sdl-config --libs)
# Insert -lSDL_mixer before -lSDL, but after any previous -L/path/to/lib entries
SDL_LIBS := $(subst $(firstword $(filter -l%,$(SDL_LIBS))),-lSDL_mixer $(firstword $(filter -l%,$(SDL_LIBS))),$(SDL_LIBS))

ifdef FHEROES2_WITH_IMAGE
SDL_LIBS := $(SDL_LIBS) -lSDL_image $(shell libpng-config --libs)
endif
else
SDL_LIBS := $(shell sdl2-config --libs)
# Insert -lSDL2_mixer before -lSDL2, but after any previous -L/path/to/lib entries
SDL_LIBS := $(subst $(firstword $(filter -l%,$(SDL_LIBS))),-lSDL2_mixer $(firstword $(filter -l%,$(SDL_LIBS))),$(SDL_LIBS))

ifdef FHEROES2_WITH_IMAGE
SDL_LIBS := $(SDL_LIBS) -lSDL2_image $(shell libpng-config --libs)
endif
endif
endif

CCFLAGS := $(CCFLAGS) $(SDL_FLAGS)
LIBS := $(SDL_LIBS) $(LIBS)

export CC CXX AR WINDRES CCFLAGS CFLAGS CXXFLAGS LDFLAGS LIBS PLATFORM

.PHONY: all clean

all:
	$(MAKE) -C thirdparty/libsmacker CCFLAGS="$(CCFLAGS_TP)" CFLAGS="$(CFLAGS_TP)" CXXFLAGS="$(CXXFLAGS_TP)"
	$(MAKE) -C engine
	$(MAKE) -C dist
ifdef FHEROES2_WITH_TOOLS
	$(MAKE) -C tools
endif
	$(MAKE) -C dist pot

clean:
	$(MAKE) -C thirdparty/libsmacker clean
	$(MAKE) -C tools clean
	$(MAKE) -C dist clean
	$(MAKE) -C engine clean
