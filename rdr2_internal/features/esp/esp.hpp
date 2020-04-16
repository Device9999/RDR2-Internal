#pragma once
#include "../../main/rdr2_main.hpp"

namespace features
{
	static bool world_to_screen(Vector3 world, float& screen_x, float& screen_y) {
		if (hooks::globals::world_to_screen(world, &screen_x, &screen_y)) {
			screen_x *= hooks::globals::resolution.right;
			screen_y *= hooks::globals::resolution.bottom;
			return true;
		}
		return false;
	}

	static bool get_bounding_box(Vector3 origin, Vector3 mins, Vector3 maxs, int& out_x, int& out_y, int& out_w, int& out_h)
	{
		Vector3 min = mins + origin;
		Vector3 max = maxs + origin;
		Vector3 points[8] = {
				Vector3(min.x, min.y, min.z),
				Vector3(min.x, max.y, min.z),
				Vector3(max.x, max.y, min.z),
				Vector3(max.x, min.y, min.z),
				Vector3(max.x, max.y, max.z),
				Vector3(min.x, max.y, max.z),
				Vector3(min.x, min.y, max.z),
				Vector3(max.x, min.y, max.z)
		};

		Vector3 w2s_points[8];
		for (int i = 0; i < 8; i++) {
			if (!world_to_screen(points[i], w2s_points[i].x, w2s_points[i].y))
				return false;
		}

		float x = w2s_points[0].x;
		float y = w2s_points[0].y;
		float width = w2s_points[0].x;
		float height = w2s_points[0].y;
		for (auto point : w2s_points) {
			if (x > point.x)
				x = point.x;
			if (width < point.x)
				width = point.x;
			if (y > point.y)
				y = point.y;
			if (height < point.y)
				height = point.y;
		}

		out_x = x;
		out_y = y;
		out_w = width - x;
		out_h = height - y;
		return true;
	}


	class c_esp
	{
	public:

		void draw_players();

	};
}