#pragma once
#include "../main/rdr2_main.hpp"


namespace helpers
{
	void request_control(int input);
	void request_control_of_id(int netid);
	void request_control_of_ent(int entity);
	std::string get_keyboard_input(const char* windowName = "", int maxInput = 150, const char* defaultText = "");
	bool is_key_pressed(int vKey);
	bool get_key_state(DWORD key, int flag);
	std::string get_clipboard_text();
}