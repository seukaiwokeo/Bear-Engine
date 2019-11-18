#pragma once
#include "stdafx.h"
class Bear
{
public:
	HANDLE hProcess;
	Bear(std::string proc)
	{
		hProcess = GetProcessByName(proc.c_str());
	}
	void kill()
	{
		TerminateProcess(hProcess, 0);
	}
	void showMsg(std::string msg)
	{
		MessageBox(NULL, msg.c_str(), "BearEngine", NULL);
	}
	DWORD getModule(std::string mName)
	{
		return (DWORD)GetModuleHandle(mName.c_str());
	}
	HANDLE GetProcessByName(PCSTR name)
	{
		DWORD pid = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 process;
		ZeroMemory(&process, sizeof(process));
		process.dwSize = sizeof(process);
		if (Process32First(snapshot, &process))
		{
			do
			{
				if (string(process.szExeFile) == string(name))
				{
					pid = process.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process));
		}
		CloseHandle(snapshot);

		if (pid != 0)
		{
			return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		}
		return NULL;
	}
	void getThreads(vector<DWORD> &threads)
	{
		DWORD PID = GetProcessId(hProcess);
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, PID);
		if (h != INVALID_HANDLE_VALUE) {
			THREADENTRY32 te;
			te.dwSize = sizeof(te);
			if (Thread32First(h, &te)) {
				do {
					if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID) && te.th32OwnerProcessID == PID) {
						threads.push_back(te.th32ThreadID);
					}
				} while (Thread32Next(h, &te));
			}
		}
	}
	void inject(char* dllFile)
	{
		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		LPVOID dereercomp = VirtualAllocEx(hProcess, NULL, strlen(dllFile), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(hProcess, dereercomp, dllFile, strlen(dllFile), NULL);
		HANDLE asdc = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
		WaitForSingleObject(asdc, INFINITE);
		VirtualFreeEx(hProcess, dereercomp, strlen(dllFile), MEM_RELEASE);
		CloseHandle(asdc);
	}

	typedef LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
	typedef LONG(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);

	void suspend()
	{
		NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(GetModuleHandle("ntdll"), "NtSuspendProcess");
		pfnNtSuspendProcess(hProcess);
		CloseHandle(hProcess);
	}
	void resume()
	{
		NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(GetModuleHandle("ntdll"), "NtResumeProcess");
		pfnNtResumeProcess(hProcess);
		CloseHandle(hProcess);
	}
	uintptr_t FindPatternEx(char* module, char* pattern, char* mask)
	{
		MODULEENTRY32 modEntry = GetModule(GetProcessId(hProcess), module);
		uintptr_t start = (uintptr_t)modEntry.modBaseAddr;
		uintptr_t end = start + modEntry.modBaseSize;

		uintptr_t currentChunk = start;
		SIZE_T bytesRead;

		while (currentChunk < end)
		{
			DWORD oldprotect;
			VirtualProtectEx(hProcess, (void*)currentChunk, 4096, PROCESS_VM_READ, &oldprotect);
			byte buffer[4096];
			ReadProcessMemory(hProcess, (void*)currentChunk, &buffer, 4096, &bytesRead);
			if (bytesRead == 0)
			{
				return 0;
			}
			uintptr_t InternalAddress = FindPattern((char*)&buffer, bytesRead, pattern, mask);
			if (InternalAddress != 0)
			{
				uintptr_t offsetFromBuffer = InternalAddress - (uintptr_t)&buffer;
				return currentChunk + offsetFromBuffer;
			}
			else
			{
				currentChunk = currentChunk + bytesRead;
			}
		}
		return 0;
	}
private:

	MODULEENTRY32 GetModule(uintptr_t dwProcID, char* moduleName)
	{
		MODULEENTRY32 modEntry = { 0 };
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			modEntry.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &modEntry))
			{
				do
				{
					if (string(modEntry.szModule) == string(moduleName))
					{
						break;
					}
				} while (Module32Next(hSnapshot, &modEntry));
			}
			CloseHandle(hSnapshot);
		}
		return modEntry;
	}
	uintptr_t FindPattern(char* base, unsigned int size, char* pattern, char* mask)
	{
		size_t patternLength = strlen(mask);

		for (uintptr_t i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (uintptr_t j = 0; j < patternLength; j++)
			{
				if (mask[j] != '?' && pattern[j] != *(char*)(base + i + j))
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return (uintptr_t)base + i;
			}
		}
		return 0;
	}
};