# ###############################
# CMake build file for Ballerburg
# ###############################

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (ballerburg C)
set(VERSION 1.2.0)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCCompilerFlag)
include(DistClean)


# Set build type to "Release" if user did not specify any build type yet
# Other possible values: Debug, Release, RelWithDebInfo and MinSizeRel
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)


# ####################
# Paths configuration:
# ####################

if(NOT BINDIR)
	set(BINDIR bin)
endif()

if(NOT DATADIR)
	set(DATADIR share/ballerburg)
endif()

if(NOT BIN2DATADIR)
	set(BIN2DATADIR "../share/ballerburg"
	    CACHE STRING "Relative path from bindir to datadir")
	mark_as_advanced(BIN2DATADIR)
endif()

if(NOT LOCALEDIR)
	set(LOCALEDIR share/locale)
endif()

if(NOT BIN2LOCALEDIR)
	set(BIN2LOCALEDIR "../share/locale"
	    CACHE STRING "Relative path from bindir to locale directory")
	mark_as_advanced(BIN2LOCALEDIR)
endif()

if(NOT MANDIR)
	set(MANDIR share/man/man6)
endif()

if(NOT DOCDIR)
	set(DOCDIR share/doc/ballerburg)
endif()


# ####################
# Check for libraries:
# ####################

if(ENABLE_SDL2)
	find_package(SDL2 REQUIRED)
	if(NOT SDL2_FOUND)
		message(FATAL_ERROR "SDL2 library not found!")
	endif(NOT SDL2_FOUND)
	set(SDL_INCLUDE_DIR ${SDL2_INCLUDE_DIR})
	set(SDL_LIBRARY ${SDL2_LIBRARY})
	set(SDLMAIN_LIBRARY ${SDL2MAIN_LIBRARY})
else(ENABLE_SDL2)
	find_package(SDL REQUIRED)
	if(NOT SDL_FOUND)
		message(FATAL_ERROR "SDL library not found!")
	endif(NOT SDL_FOUND)
endif(ENABLE_SDL2)

find_package(Math REQUIRED)
find_package(Gettext)

# ################
# CPP Definitions:
# ################

# Additional CFLAGS suggested by the SDL library:
if(ENABLE_SDL2)
	add_definitions(-DWITH_SDL2)
	execute_process(COMMAND pkg-config --cflags-only-other sdl2
			OUTPUT_VARIABLE DETECTED_SDL_CFLAGS
			ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
else(ENABLE_SDL2)
	execute_process(COMMAND pkg-config --cflags-only-other sdl
			OUTPUT_VARIABLE DETECTED_SDL_CFLAGS
			ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(ENABLE_SDL2)
if(DETECTED_SDL_CFLAGS)
	add_definitions(${DETECTED_SDL_CFLAGS})
	# message(STATUS "Additional CFLAGS of SDL: ${DETECTED_SDL_CFLAGS}")
endif(DETECTED_SDL_CFLAGS)

# ###########################
# Check for optional headers:
# ###########################

check_include_files(libintl.h HAVE_LIBINTL_H)


# ###############
# Various CFLAGS:
# ###############

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsigned-char -Wall")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra -Wno-unused-parameter")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes -Wstrict-prototypes")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wbad-function-cast")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat-security -Wpointer-arith")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-qual -Wshadow")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2")
	endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif()


# #########################################
# Create config.h and recurse into subdirs:
# #########################################

configure_file(${CMAKE_SOURCE_DIR}/cmake/config-cmake.h
               ${CMAKE_BINARY_DIR}/config.h)

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(po)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/Uninstall.cmake)
