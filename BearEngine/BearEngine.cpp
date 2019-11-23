#include "Bear.h"


Bear* l_CheckBear(lua_State* l, int n)
{
	return *(Bear**)luaL_checkudata(l, n, "luaL_Bear");
}

int l_Bear_constructor(lua_State* l)
{
	const char* name = luaL_checkstring(l, 1);
	int it = luaL_checkunsigned(l, 2);
	Bear** udata = (Bear**)lua_newuserdata(l, sizeof(Bear*));
	*udata = new Bear(name, it);
	luaL_getmetatable(l, "luaL_Bear");
	lua_setmetatable(l, -2);
	return 1;
}

int l_Bear_msg(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string msg = luaL_checkstring(l, 2);
	bear->showMsg(msg);
	return 1;
}

int l_Bear_getModule(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string arg1 = luaL_checkstring(l, 2);
	DWORD mdl = bear->getModule(arg1);
	lua_pushinteger(l, mdl);
	return 1;
}

int l_Bear_getProcess(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string arg1 = luaL_checkstring(l, 2);
	unsigned int arg2 = luaL_checkunsigned(l, 3);
	HANDLE proc = bear->GetProcessByName(arg1.c_str(), arg2);
	lua_pushinteger(l, (int)proc);
	return 1;
}

int l_Bear_findPattern(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string mdlArg = luaL_checkstring(l, 2);
	char* mdl = (char*)mdlArg.c_str();
	unsigned char* pattern = (unsigned char*)luaL_checklstring(l, 3, NULL);
	unsigned char* mask = new unsigned char[strlen((const char*)pattern)];
	for (int i = 0; i < strlen((const char*)pattern); i++)
	{
		if (pattern[i] == 0x0 || pattern[i] == '0') mask[i] = '?';
		else mask[i] = 'x';
	}
	DWORD address = bear->FindPatternEx(mdl, (char*)pattern, (char*)mask);
	lua_pushunsigned(l, address);
	return 1;
}

int l_Bear_asmPatch(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string mdlArg = luaL_checkstring(l, 2);
	char* mdl = (char*)mdlArg.c_str();
	unsigned char* pattern = (unsigned char*)luaL_checklstring(l, 3, NULL);
	unsigned char* replace = (unsigned char*)luaL_checklstring(l, 4, NULL);
	unsigned char* mask = new unsigned char[strlen((const char*)pattern)];
	for (int i = 0; i < strlen((const char*)pattern); i++)
	{
		if (pattern[i] == 0x0 || pattern[i] == '0') mask[i] = '?';
		else mask[i] = 'x';
	}
	DWORD address = bear->FindPatternEx(mdl, (char*)pattern, (char*)mask);
	WriteProcessMemory(bear->hProcess, (LPVOID)address, replace, strlen((const char*)replace), 0);
	lua_pushboolean(l, true);
	return 1;
}

int l_Bear_asmPatchEx(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string mdlArg = luaL_checkstring(l, 2);
	char* mdl = (char*)mdlArg.c_str();
	DWORD address = luaL_checkunsigned(l, 3);
	unsigned char* pattern = (unsigned char*)luaL_checklstring(l, 4, NULL);
	WriteProcessMemory(bear->hProcess, (LPVOID)address, pattern, strlen((const char*)pattern), 0);
	lua_pushboolean(l, true);
	return 1;
}

int l_Bear_readMemory(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	int readas = luaL_checkint(l, 3);
	if (readas == 1) {
		BYTE ret;
		ReadProcessMemory(bear->hProcess, (LPVOID)address, &ret, 1UL, 0);
		lua_pushbyte(l, ret);
	}
	else if (readas == 2) {
		__int16 ret;
		ReadProcessMemory(bear->hProcess, (LPVOID)address, &ret, 2UL, 0);
		lua_pushint16(l, ret);
	}
	else if (readas == 4) {
		__int32 ret;
		ReadProcessMemory(bear->hProcess, (LPVOID)address, &ret, 4UL, 0);
		lua_pushinteger(l, ret);
	}
	else if (readas == 8) {
		__int64 ret;
		ReadProcessMemory(bear->hProcess, (LPVOID)address, &ret, 8UL, 0);
		lua_pushint64(l, ret);
	}
	else if (readas == -4) {
		float ret;
		ReadProcessMemory(bear->hProcess, (LPVOID)address, &ret, sizeof(ret), 0);
		lua_pushfloat(l, ret);
	}
	else if (readas == -8) {
		double ret;
		ReadProcessMemory(bear->hProcess, (LPVOID)address, &ret, sizeof(ret), 0);
		lua_pushdouble(l, ret);
	}
	else lua_pushinteger(l, 0);
	return 1;
}

int l_Bear_writeMemory(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	lua_Number value = luaL_checknumber(l, 3);
	WriteProcessMemory(bear->hProcess, (LPVOID)address, &value, 4, 0);
	return 1;
}

int l_Bear_readString(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	int size = luaL_checkunsigned(l, 3);
	const char* value = new char[size];
	ReadProcessMemory(bear->hProcess, (LPVOID)address, &value, size, 0);
	lua_pushstring(l, value);
	return 1;
}

int l_Bear_writeString(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	std::string strArg = luaL_checkstring(l, 3);
	const char* value = strArg.c_str();
	WriteProcessMemory(bear->hProcess, (LPVOID)address, &value, strlen(value), 0);
	return 1;
}

int l_Bear_killThread(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, address);
	TerminateThread(hThread, 0);
	return 1;
}

int l_Bear_suspendThread(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, address);
	SuspendThread(hThread);
	return 1;
}

int l_Bear_resumeThread(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD address = luaL_checkunsigned(l, 2);
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, address);
	ResumeThread(hThread);
	return 1;
}

int l_Bear_suspend(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	bear->suspend();
	return 1;
}

int l_Bear_resume(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	bear->resume();
	return 1;
}

int l_Bear_sleep(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	DWORD timer = luaL_checkunsigned(l, 2);
	bear->suspend();
	Sleep(timer);
	bear->resume();
	return 1;
}

int l_Bear_load(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string file = luaL_checkstring(l, 2);
	int erred = luaL_dofile(l, file.c_str());
	if (erred)
		std::cout << "Lua error: " << luaL_checkstring(l, -1) << std::endl;

	lua_close(l);
	return 1;
}

int l_Bear_kill(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	bear->kill();
	return 1;
}

int l_Bear_loadLib(lua_State* l)
{
	Bear* bear = l_CheckBear(l, 1);
	std::string file = luaL_checkstring(l, 2);
	bear->inject((char*)file.c_str());
	return 1;
}

int l_Bear_getThread(lua_State * l)
{
	Bear* bear = l_CheckBear(l, 1);
	vector<DWORD> threads;
	bear->getThreads(threads);
	lua_createtable(l, threads.size(), 0);
	int newTable = lua_gettop(l);
	int index = 1;
	for (unsigned int i : threads)
	{
		lua_pushunsigned(l, i);
		lua_rawseti(l, newTable, index);
		++index;
	}
	return 1;
}

void RegisterBear(lua_State* l)
{
	luaL_Reg bearRegs[] =
	{
		{ "init", l_Bear_constructor },
		{ "msg", l_Bear_msg },
		{ "kill", l_Bear_kill},
		{ "suspend", l_Bear_suspend },
		{ "resume", l_Bear_resume },
		{ "sleep", l_Bear_sleep },
		{ "load", l_Bear_load },
		{ "loadLib", l_Bear_loadLib },
		{ "killThread", l_Bear_killThread },
		{ "suspendThread", l_Bear_suspendThread },
		{ "resumeThread", l_Bear_resumeThread },
		{ "getProcess", l_Bear_getProcess },
		{ "getModule", l_Bear_getModule },
		{ "findPattern", l_Bear_findPattern },
		{ "asmPatch", l_Bear_asmPatch },
		{ "asmPatchEx", l_Bear_asmPatchEx },
		{ "writeMemory", l_Bear_writeMemory },
		{ "readMemory", l_Bear_readMemory },
		{ "writeString", l_Bear_writeString },
		{ "readString", l_Bear_readString },
		{ "getThreads", l_Bear_getThread },
		{ NULL, NULL }
	};
	luaL_newmetatable(l, "luaL_Bear");
	luaL_setfuncs(l, bearRegs, 0);
	lua_pushvalue(l, -1);
	lua_setfield(l, -1, "__index");
	lua_setglobal(l, "Bear");
}

void getScripts(vector<std::string> &scripts)
{
	std::string pattern("scripts");
	std::string base = pattern;
	pattern.append("\\*.lua");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			scripts.push_back(base + "\\" + string(data.cFileName));
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

void main() {
	SetConsoleTitle("BearEngine");
	lua_State* l = luaL_newstate();
	luaL_openlibs(l);
	RegisterBear(l);

	vector<string> scripts;
	getScripts(scripts);

	for (string sc : scripts) {
		int erred = luaL_dofile(l, sc.c_str());
		if (erred)
			std::cout << "Lua error: " << luaL_checkstring(l, -1) << std::endl;
	}

	lua_close(l);
	system("pause");
}