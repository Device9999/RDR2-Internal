#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h> 
#include <algorithm>
#include <vector>
#include <string>
#include <Psapi.h>
#include <map>
#include <time.h>
#include <DirectXMath.h>
#pragma comment(lib, "winmm.lib")
#include "../invoker/settings.hpp"
#include "../invoker/invoker.hpp"
#include "../invoker/natives.hpp"
#include "../helpers/helpers.hpp"
#include "../renderer/renderer.hpp"
#include "../libs/Minhook/include/MinHook.h"
#include "../memory/memory.hpp"

#define M_PI       3.14159265358979323846
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( M_PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( M_PI ) ) )

typedef BOOL(__fastcall* is_dlc_present_fn)(__int64 a1, __int64 a2);
typedef int(__fastcall* dispatch_report_fn)(__int64 playerindex, __int64 a2, char* description_of_report, char* horse_name);

typedef enum _MEMORY_INFORMATION_CLASS {
	MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;

typedef NTSTATUS(NTAPI* NtQueryVirtualMemory_fn)(HANDLE ProcessHandle,
	PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass,
	PVOID Buffer, ULONG Length, PULONG ResultLength);
using w2s_fn = bool(*)(Vector3 world, float* x, float* y);
struct input_screen_t {
	std::string title = "";
	bool drawing = false;
};

extern c_settings settings;

namespace fiber
{
	extern void wait_for(DWORD ms);
}

namespace hooks
{
	namespace globals
	{
		extern RECT resolution;
		extern bool menu_active;
		extern uintptr_t  base_address;
		extern w2s_fn world_to_screen;
		extern input_screen_t showing_keyboard_input_screen;
		extern bool draw_delete_option;
		extern std::vector<int> delete_entities;
		extern WNDPROC o_wndproc;
		extern bool key_pressed[256];
	}

	namespace original
	{
		extern is_dlc_present_fn o_does_cam_exist;
		extern dispatch_report_fn o_dispatch_report;
		extern NtQueryVirtualMemory_fn o_ntqvm;
	}

	extern void __fastcall dispatch_report(__int64 playerindex, __int64 a2, char* description_of_report, char* horse_name);

	extern LRESULT __stdcall WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	extern BOOL __fastcall does_cam_exist_hook(__int64 a1, __int64 a2);

	extern 	NTSTATUS NTAPI NtQueryVirtualMemory_hook(
		IN HANDLE ProcessHandle,
		IN PVOID BaseAddress,
		IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
		OUT PVOID Buffer,
		IN ULONG Length,
		OUT PULONG ResultLength OPTIONAL);

	extern void initialize();

}