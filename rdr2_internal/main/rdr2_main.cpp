#include "rdr2_main.hpp"
#include "../features/esp/esp.hpp"
#include "../menu/menu.hpp"
#include "../features/features.hpp"

c_menu_framework* menu_framework = new c_menu_framework;
c_settings settings;

namespace fiber {
	static HANDLE main_fiber = nullptr;
	static DWORD time_to_wake_up = 0;

	void wait_for(DWORD ms) {
		time_to_wake_up = timeGetTime() + ms;
		SwitchToFiber(main_fiber);
	}

	void __stdcall fiber_thread(LPVOID params) {
		srand(GetTickCount64());
		while (true) {

			//call shit here
			features::c_features().on_tick();

			wait_for(0);
		}
	}

	void on_tick() {
		if (!main_fiber)
			main_fiber = ConvertThreadToFiber(nullptr);

		if (time_to_wake_up > timeGetTime())
			return;

		static HANDLE fiber_handle = nullptr;
		if (fiber_handle) {
			SwitchToFiber(fiber_handle);
		}
		else {
			fiber_handle = CreateFiber(NULL, fiber_thread, nullptr);
		}
	}
}

namespace hooks
{
	namespace globals
	{
		RECT resolution;
		bool menu_active = false;
		uintptr_t base_address;
		w2s_fn world_to_screen;
		input_screen_t showing_keyboard_input_screen;
		bool draw_delete_option = false;
		std::vector<int> delete_entities;
		WNDPROC o_wndproc;
		bool key_pressed[256] = {};
	}

	namespace original
	{
		is_dlc_present_fn o_does_cam_exist;
		dispatch_report_fn o_dispatch_report;
		NtQueryVirtualMemory_fn o_ntqvm;
	}
	
	void __fastcall dispatch_report(__int64 playerindex, __int64 a2, char* description_of_report, char* horse_name) // only gets called when localplayer reports someone
	{
		auto get_report_name_from_type = [](int type) {
			int v1; // ecx
			int v2; // ecx
			int v3; // ecx
			int v4; // ecx
			int v5; // ecx
			int v6; // ecx
			int v7; // ecx

			if (!type)
				return "cheating";
			v1 = type - 1;
			if (!v1)
				return "abusive chat";
			v2 = v1 - 1;
			if (!v2)
				return "disruptive behavior";
			v3 = v2 - 1;
			if (!v3)
				return "offensive level name";
			v4 = v3 - 1;
			if (!v4)
				return "offensive social club name";
			v5 = v4 - 1;
			if (!v5)
				return "offensive crew name";
			v6 = v5 - 1;
			if (!v6)
				return "offensive crew emblem";
			v7 = v6 - 1;
			if (!v7)
				return "offensive posse name";
			if (v7 == 1)
				return "offensive horse name";
			return "ERROR";
		};

		auto report_id = get_report_name_from_type(a2);
		return;
	}

	LRESULT __stdcall WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg) {
		case WM_LBUTTONDOWN:
			//printf("l_button\n");
			globals::key_pressed[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			globals::key_pressed[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			globals::key_pressed[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			globals::key_pressed[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			globals::key_pressed[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			globals::key_pressed[VK_MBUTTON] = false;
			break;
		case WM_XBUTTONDOWN:
		{
			UINT button = GET_XBUTTON_WPARAM(wparam);
			if (button == XBUTTON1)
			{
				globals::key_pressed[VK_XBUTTON1] = true;
			}
			else if (button == XBUTTON2)
			{
				globals::key_pressed[VK_XBUTTON2] = true;
			}
			break;
		}
		case WM_XBUTTONUP:
		{
			UINT button = GET_XBUTTON_WPARAM(wparam);
			if (button == XBUTTON1)
			{
				globals::key_pressed[VK_XBUTTON1] = false;
			}
			else if (button == XBUTTON2)
			{
				globals::key_pressed[VK_XBUTTON2] = false;
			}
			break;
		}
		case WM_KEYDOWN:
			globals::key_pressed[wparam] = true;
			break;
		case WM_KEYUP:
			globals::key_pressed[wparam] = false;
			break;
		default: break;
		}

		return CallWindowProc(globals::o_wndproc, hwnd, msg, wparam, lparam);
	}


	BOOL __fastcall does_cam_exist_hook(__int64 a1, __int64 a2) {

		static bool opened_before = false;
		if (!opened_before)
			rendering::c_renderer::get()->draw_text(20, 5, 19.f, "Press [INSERT] Key to open menu.", 255, 0, 0, 255);

		//static bool menu_open = false;
		if (helpers::get_key_state(VK_INSERT, 1))
			globals::menu_active = !globals::menu_active;

		if (globals::menu_active) {
			if (!opened_before)
				opened_before = true;

			menu_framework->add_menu_entries(20, 40);
		}

		globals::draw_delete_option = !(globals::delete_entities.empty());

		fiber::on_tick();

		if (globals::showing_keyboard_input_screen.drawing) {
			auto title = globals::showing_keyboard_input_screen.title;
			if (!title.empty())
				rendering::c_renderer::get()->draw_text((hooks::globals::resolution.right / 2) - 280, 230, 27.f, title.c_str(), 255, 180, 180, 255);
		}

		features::c_esp().draw_players();

		if (globals::menu_active)
			menu_framework->draw_menu_entries();


		return original::o_does_cam_exist(a1, a2);
	}


#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)
	NTSTATUS NTAPI NtQueryVirtualMemory_hook(
		IN HANDLE ProcessHandle,
		IN PVOID BaseAddress,
		IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
		OUT PVOID Buffer,
		IN ULONG Length,
		OUT PULONG ResultLength OPTIONAL)
	{
		return STATUS_ACCESS_DENIED;
	}

	void initialize() {

		if (AllocConsole()) {
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}
		printf("Cheat loaded!\n\n");
		globals::base_address = uintptr_t(GetModuleHandleA(0));
		auto hwnd_ = FindWindowA(0, "Red Dead Redemption 2");
		GetWindowRect(hwnd_, &globals::resolution);
		auto width = (float)hooks::globals::resolution.right,
			height = (float)hooks::globals::resolution.bottom;


		globals::o_wndproc = (WNDPROC)SetWindowLongPtrA(hwnd_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

		globals::world_to_screen = reinterpret_cast<w2s_fn>(memory::find_signature(0, "\x48\x89\x5C\x24\x00\x56\x57\x41\x56\x48\x83\xEC\x60", "xxxx?xxxxxxxx"));
		
		if (MH_Initialize() == MH_OK) {
			MH_CreateHookApi(L"ntdll.dll", "NtQueryVirtualMemory", NtQueryVirtualMemory_hook, reinterpret_cast<void**>(&original::o_ntqvm));
			auto does_cam_exist = (void*)get_handler(0x153AD457764FD704);
			MH_CreateHook(does_cam_exist, does_cam_exist_hook, reinterpret_cast<void**>(&original::o_does_cam_exist));
			MH_EnableHook(MH_ALL_HOOKS);
		}
	}
}

