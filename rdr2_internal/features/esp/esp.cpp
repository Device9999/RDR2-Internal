#include "esp.hpp"

#include <string>
namespace features
{
	void c_esp::draw_players() {
		auto local_ped_id = PLAYER::PLAYER_PED_ID();
		auto local_pos = ENTITY::GET_ENTITY_COORDS(local_ped_id, 0, 0);

		for (int i = 0; i < 32; i++) {
			auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i);
			if (!ENTITY::DOES_ENTITY_EXIST(ped) || ped == local_ped_id)
				continue;

			auto  max_health = ENTITY::GET_ENTITY_MAX_HEALTH(ped, 0);

			auto health = ENTITY::GET_ENTITY_HEALTH(ped);
			if ((health <= 0) || ENTITY::IS_ENTITY_DEAD(ped))
				continue;

			auto position = ENTITY::GET_ENTITY_COORDS(ped, false, false);
			//Vector3 mins, maxs;
			//MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(ped), &mins, &maxs); // doesn't seem to return correct values,
			auto mins = Vector3(-0.61f, -0.25f, -1.30f), maxs = Vector3(0.61f, 0.25f, 0.94f); // use c_ped->c_model_info-> vec_mins/vec_maxs
			int x = 0, y = 0, w = 0, h = 0;
			if (get_bounding_box(position, mins, maxs, x, y, w, h)) {
				auto is_god_mode = PLAYER::GET_PLAYER_INVINCIBLE(i);
				std::string name = "";
				if (settings.esp.draw_name || settings.esp.draw_distance) {

					if(is_god_mode)
						rendering::c_renderer::get()->draw_text(x + (w / 2) - 6, y - 20, 14.f, "GODMODE", 255, 0, 0, 255);

					if (settings.esp.draw_name)
						name = PLAYER::GET_PLAYER_NAME(i);

					if (settings.esp.draw_distance) {
						if (settings.esp.draw_name)
							name += " | ";
						name += std::to_string(int(position.dist_to(local_pos)));
						name += "m";
					}
					rendering::c_renderer::get()->draw_text(x + (w / 2) - (name.length()), y - 13, 14.f, name, 255, 255, 255, 255);
				}

				if (settings.esp.draw_box) {
					rendering::c_renderer::get()->draw_rect(x, y, w, h, 1.f, 0, 0, 0, 255);
					rendering::c_renderer::get()->draw_rect(x + 1, y + 1, w - 2, h - 2, 1.f, 255, 0, 0, 255);
					rendering::c_renderer::get()->draw_rect(x + 2, y + 2, w - 4, h - 4, 1.f, 0, 0, 0, 255);
				}
				if (settings.esp.draw_health) {
					float hr = 255 - (health * 2.55f);
					float hg = health * 2.55f;

					rendering::c_renderer::get()->draw_filled_rect(x, y + h + 2, (health * w / ENTITY::GET_ENTITY_MAX_HEALTH(ped, 0)), 5, hr, hg, 0, 255);
					rendering::c_renderer::get()->draw_rect(x, y + h + 2, w, 5, 1.f,  0, 0, 0, 255);
				}

			}
		}
	}

}