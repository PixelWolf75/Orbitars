cmake_minimum_required(VERSION 3.13)
project(Orbitars_with_Catch)

set(CMAKE_CXX_STANDARD 20)

include_directories("Orbitars with Catch")
include_directories("Orbitars with Catch/catch2")
include_directories("Orbitars with Catch/sql")
include_directories(Source)
include_directories(Source/Debug)
find_library(GDIPLUS_LIBRARY NAMES libgdiplus gdiplus)
set(GDIPLUS_LIBRARY gdiplus)
add_executable(Orbitars_with_Catch
        "Orbitars with Catch/catch2/catch.hpp"
        "Orbitars with Catch/sql/ISQLDatabase.cpp"
        "Orbitars with Catch/sql/ISQLDatabase.h"
        "Orbitars with Catch/sql/sqlite3.c"
        "Orbitars with Catch/sql/sqlite3.h"
        "Orbitars with Catch/sql/sqlite3ext.h"
        "Orbitars with Catch/Asteroid.cpp"
        "Orbitars with Catch/Asteroid.h"
        "Orbitars with Catch/Bullet.cpp"
        "Orbitars with Catch/Bullet.h"
        "Orbitars with Catch/Catch2_Testing.cpp"
        "Orbitars with Catch/Entity.cpp"
        "Orbitars with Catch/Entity.h"
        "Orbitars with Catch/framework.h"
        "Orbitars with Catch/Game.cpp"
        "Orbitars with Catch/GameState.h"
        "Orbitars with Catch/IQuadTree.cpp"
        "Orbitars with Catch/IQuadTree.h"
        "Orbitars with Catch/IScreen.cpp"
        "Orbitars with Catch/IScreen.h"
        "Orbitars with Catch/IStars.cpp"
        "Orbitars with Catch/IStars.h"
        "Orbitars with Catch/Main.cpp"
        "Orbitars with Catch/Orbitars with Catch.cpp"
        "Orbitars with Catch/Orbitars with Catch.h"
        "Orbitars with Catch/Player.cpp"
        "Orbitars with Catch/Player.h"
        "Orbitars with Catch/Resource.h"
        "Orbitars with Catch/Splash.cpp"
        "Orbitars with Catch/targetver.h"
        "Orbitars with Catch/Universal_Structures.h"
        Source/Debug/Source.pch
        Source/framework.h
        Source/pch.cpp
        Source/pch.h
        Source/Source.cpp
        Test/Test.cpp)
