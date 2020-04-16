#include "helpers.hpp"

namespace helpers
{
	void request_control(int input) {
		NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(input);
		int tick = 0;
		while (tick <= 50) {
			if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(input))
				fiber::wait_for(0);
			else
				return;
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(input);
			tick++;
		}
	}

	void request_control_of_id(int netid) {
		int tick = 0;
		while (!NETWORK::NETWORK_HAS_CONTROL_OF_NETWORK_ID(netid) && tick <= 25) {
			NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(netid);
			tick++;
		}
	}

	void request_control_of_ent(int entity) {
		int tick = 0;
		while (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity) && tick <= 25) {
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
			tick++;
		}
		if (NETWORK::NETWORK_IS_SESSION_STARTED()) {
			int netID = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(entity);
			request_control_of_id(netID);
			//NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, 1);
		}
	}

	std::string get_keyboard_input(const char* windowName, int maxInput, const char* defaultText) {
		std::string defaultText_ = defaultText;
		hooks::globals::showing_keyboard_input_screen.drawing = true;
		hooks::globals::showing_keyboard_input_screen.title = windowName;
		start:
		MISC::DISPLAY_ONSCREEN_KEYBOARD(0, windowName, "", defaultText_.c_str(), "", "", "", maxInput);
		while (MISC::UPDATE_ONSCREEN_KEYBOARD() == 0) {
			if (GetAsyncKeyState(VK_LCONTROL) && is_key_pressed('V')) {
				auto keybd = get_clipboard_text();
				if (!keybd.empty()) {
					defaultText_ = keybd;
					MISC::_CANCEL_ONSCREEN_KEYBOARD();
					goto start;
				}
			}

			fiber::wait_for(0);
		}
		if (!MISC::GET_ONSCREEN_KEYBOARD_RESULT()) return "";
		hooks::globals::showing_keyboard_input_screen.drawing = false;
		return MISC::GET_ONSCREEN_KEYBOARD_RESULT();
	}
	bool is_key_pressed(int vKey) {
		static bool was_pressed[300];

		bool is_pressed = (GetAsyncKeyState(vKey) & 0x8000) != 0;
		if (!was_pressed[vKey] && is_pressed) {
			was_pressed[vKey] = is_pressed;
			return true;
		}
		was_pressed[vKey] = is_pressed;
		return false;
	}

	bool get_key_state(DWORD key, int flag) // from sub1to's gta internal
	{
		clock_t	c = clock();
		static BYTE			btKeyState[0x100] = { 0 };
		static clock_t		clockKeyState[0x100] = { c };

		bool	r = false;
		BYTE	btState = (GetAsyncKeyState(key) & 0x8000U) >> 0xF;
		short	wDelay = btKeyState[key] & 2 ? 0x50 : 0x200;
		if (btState) {
			if (flag & 1) {
				if (!(btKeyState[key] & 1) || c - clockKeyState[key] > wDelay)
				{
					if ((btKeyState[key] & 3) == 1)
						btKeyState[key] |= 2;
					r = true;
					clockKeyState[key] = c;
				}
			}
			else
				r = true;
		}
		else
			btKeyState[key] &= ~(2);
		btKeyState[key] ^= (-btState ^ btKeyState[key]) & 1;

		return r;
	}

	std::string get_clipboard_text()
	{
		if (!OpenClipboard(nullptr))
			return "";

		HANDLE data = GetClipboardData(CF_TEXT);
		if (data == nullptr)
			return "";

		char* pszText = static_cast<char*>(GlobalLock(data));
		if (pszText == nullptr)
			return "";

		std::string text(pszText);
		GlobalUnlock(data);
		CloseClipboard();

		return text;
	}

}