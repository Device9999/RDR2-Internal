// dllmain.cpp : Defines the entry point for the DLL application.
#include "main/rdr2_main.hpp"


unsigned long __stdcall on_attach()
{
	hooks::initialize();
	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)on_attach, hModule, 0, 0);
	}
	return 1;
}
