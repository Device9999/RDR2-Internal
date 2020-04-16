#include "renderer.hpp"

namespace rendering
{
	void c_renderer::draw_filled_rect(float x, float y, float width, float height, int r, int g, int b, int a)
	{
		auto res_w = (float)(hooks::globals::resolution.right),
			res_h = (float)(hooks::globals::resolution.bottom);

		float fWidth = width / res_w;
		float fHeight = height / res_h;
		float fX = (x + width / 2) / res_w;
		float fY = (y + height / 2) / res_h;

		GRAPHICS::DRAW_RECT(fX, fY, fWidth, fHeight, r, g, b, a, true, 0);
	}

	void c_renderer::draw_rect(int x, int y, int w, int h, float thickness, int r, int g, int b, int a) {
		this->draw_filled_rect(x, y, thickness, h, r, g, b, a); // LEFT TOP TO LEFT BOTTOM
		this->draw_filled_rect(x, y, w, thickness, r, g, b, a); // LEFT TOP TO RIGHT TOP
		this->draw_filled_rect(x, y + h, w, thickness, r, g, b, a); //LEFT BOTTOM TO RIGHT BOTTOM
		this->draw_filled_rect(x + w, y, thickness, h, r, g, b, a); //RIGHT TOP TO RIGHT BOTTOM

	}

	void c_renderer::draw_text(float x, float y, float font_size, std::string text, int r, int g, int b, int a) {
		x /= hooks::globals::resolution.right; y /= hooks::globals::resolution.bottom;
		font_size /= 100.f;
		HUD::SET_TEXT_SCALE(font_size, font_size);
		HUD::_SET_TEXT_COLOR(r, g, b, a);
		auto str = MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", text.c_str());
		HUD::_DRAW_TEXT(str, x, y);
	}


}