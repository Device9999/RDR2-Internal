#pragma once
#include "../main/rdr2_main.hpp"


namespace rendering
{
	class c_renderer
	{
	public:
		static c_renderer* get() {
			static c_renderer* instance = new c_renderer;
			return instance;
		}
	public:

		void draw_filled_rect(float x, float y, float width, float height, int r, int g, int b, int a);

		void draw_rect(int x, int y, int w, int h, float thickness, int r, int g, int b, int a);

		void draw_text(float x, float y, float font_size, std::string text, int r, int g, int b, int a);

	};
}