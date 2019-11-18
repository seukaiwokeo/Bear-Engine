#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <Psapi.h>
#include <vector>

extern "C" {
#include "LuaEngine/lua.h"
#include "LuaEngine/lualib.h"
#include "LuaEngine/lauxlib.h"
}

#include "lua_helpers.h"


using namespace std;

struct thread
{
	string moduleName;
	DWORD address;
};