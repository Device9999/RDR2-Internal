#include "menu.hpp"
#include <string>

void c_menu_framework::add_menu_entries(int x, int y) {
	this->menu_x = x;
	this->menu_y = y;

	static bool player_tab = false;
	static bool horse_tab = false;
	static bool esp_tab = false;
	static bool weapon_tab = false;
	static bool replacer_tab = false;
	static bool color_tab = false;
	static bool menu_tab = false;
	static bool spawner_tab = false;
	static bool world_tab = false;

	if (!player_tab && !weapon_tab && !horse_tab && !spawner_tab && !esp_tab) {
		this->add_entry("PLAYER", &player_tab, true);
		this->add_entry("WEAPON", &weapon_tab, true);
		this->add_entry("HORSE", &horse_tab, true);
		this->add_entry("SPAWNER", &spawner_tab, true);
		this->add_entry("ESP", &esp_tab, true);

	}

	if (player_tab) {
		this->add_entry("RETURN", &player_tab, true);
		if (!player_tab)
			this->m_current_pos = 0;

		this->add_entry("GodMode", &settings.player.god_mode, 0, "You don't take any damage.");
		this->add_entry("Semi-GodMode", &settings.player.semi_godmode, 0, "Your health regenerates instantly, high damage will kill.");
		//this->add_entry("Health Recharge Speed", &settings.player.health_recharge_speed, 0.f, 500.f, 1.f, "Speed of your health recharging.");
		//this->add_entry("Swim Speed", &settings.player.swim_speed, 0.f, 500.f, 1.f, "Multiplies the speed of swimming.");
		this->add_entry("Super Jump", &settings.player.super_jump, 0, "You will be able to jump higher.");
		this->add_entry("Run Speed multiplier", &settings.player.run_speed_multiplier, 0, 25, 1, "You will be able to run faster.");
		this->add_entry("Invisible(!)", &settings.player.invisible, 0, "You become invisible, This will remove all your clothes.");
		this->add_entry("Never Wanted", &settings.player.never_wanted, 0, "You will never be wanted, just like IRL.");
		this->add_entry("Disable Ragdoll", &settings.player.disable_ragdoll, 0, "Disables your ragdoll animations.");
		this->add_entry("Infinite Stamina", &settings.player.infinite_stamina, 0, "You don't run out of stamina.");
		this->add_entry("Infinite Deadeye", &settings.player.infinite_deadeye, 0, "You don't run out of deadeye.");
		//this->add_entry("Explode All", &settings.player.explode_all, 0, "Explodes all players.");
		this->add_entry("Teleport to Waypoint", &settings.player.teleport_to_waypoint, 0, "Teleports you to the set waypoint.");
		this->add_entry("Triggerbot", &settings.player.trigger_bot, 0, "Automaticly shoots Ped in crosshair | Hold Mouse5");
		this->add_entry("Noclip", &settings.player.noclip, 0, "");
		this->add_entry("Namechanger", &settings.player.name_changer, 0, "Change your Online name temporarily.");
		this->add_entry("Modelchanger", &settings.player.model_changer, 0, "Change your Online player model temporarily.");
		this->add_entry("Ignored by NPC's", &settings.player.every_ignore, 0, "NPC's will ignore you.");
	}

	if (weapon_tab) {
		this->add_entry("RETURN", &weapon_tab, true);
		if (!weapon_tab)
			this->m_current_pos = 1;

		this->add_entry("Infinite Ammo", &settings.weapon.infinite_ammo, 0, "Your weapon doesn't run out of ammo.");
		this->add_entry("Improved Accuracy", &settings.weapon.perfect_accuracy, 0, "Your weapon's accuracy is increased.");
	

		//this->add_entry("Explosive Ammo", &settings.weapon.explosive_ammo, 0, "Your bullets explode on impact.");
		//this->add_entry("Rapid fire", &settings.weapon.rapid_fire, 0, "Increases your weapons fire rate.");
		//this->add_entry("Damage modifier", &settings.weapon.weapon_damage, 0.f, 999.f, 1.f, "Multiplies the damage of your weapon.");
		//this->add_entry("Get All Weapons", &settings.weapon.get_all_weapons, 0, "Gives you all available weapons.");
	}

	if (horse_tab) {
		this->add_entry("RETURN", &horse_tab, true);
		if (!horse_tab)
			this->m_current_pos = 2;

		this->add_entry("GodMode", &settings.horse.god_mode, 0, "Your horse doesn't take any damage.");
		this->add_entry("Infinite Stamina", &settings.horse.infinite_stamina, 0, "Your horse doesn't run out of stamina.");
		this->add_entry("Invisible", &settings.horse.invisible, 0, "Your horse becomes invisible.");
	}
	if (spawner_tab) {
		this->add_entry("RETURN", &spawner_tab, true);
		if (!spawner_tab)
			this->m_current_pos = 3;
		this->add_entry("Freeze Models", &settings.spawner.spawn_as_frozen, 0, "Makes spawned models frozen.");
		this->add_entry("Spawn Treasure Chest", &settings.spawner.spawn_gold_chest, 0, "Spawns Treasure Chest with 50x reward");
		this->add_entry("Spawn AmbientPickup", &settings.spawner.spawn_ambientpickup, 0, "Spawns AmbientPickup using model & pickup name");
		this->add_entry("Spawn Vehicle", &settings.spawner.spawn_vehicle, 0, "Spawns Vehicle using model name.");
		this->add_entry("Spawn Object", &settings.spawner.spawn_object, 0, "Spawns Object using model name.");
		this->add_entry("Spawn Ped", &settings.spawner.spawn_ped, 0, "Spawns Ped using model name.");
		this->add_entry("Spawn Dead Ped", &settings.spawner.spawn_dead_ped, 0, "Spawns Dead Ped using model name.");
		if (hooks::globals::draw_delete_option) {
			this->add_entry("Delete all spawned models", &settings.spawner.delete_spawned_models, 0, "Delete's all models you have spawned.");
		}
	}
	if (esp_tab) {
		this->add_entry("RETURN", &esp_tab, true);
		if (!esp_tab)
			this->m_current_pos = 4;
		this->add_entry("Name", &settings.esp.draw_name, 0, "Draws players name.");
		this->add_entry("Distance", &settings.esp.draw_distance, 0, "Draws players distance.");
		this->add_entry("Box", &settings.esp.draw_box, 0, "Draws box around players.");
		this->add_entry("Health", &settings.esp.draw_health, 0, "Draws players health.");
	}

}

void c_menu_framework::draw_menu_entries() {
	static int item_gap = 16;
	int alpha = 255;
	float font_size = 19.f;

	rendering::c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y - 17, 154 * 2, 17, 26, 26, 26, alpha);
	rendering::c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y - 17, 154 * 2, 17, 1.f, 255, 40, 40, alpha);

	rendering::c_renderer::get()->draw_text(this->menu_x + (154 / 2) + 10, this->menu_y - 15, font_size, "ModMenu by device9999", 255, 255, 255, alpha);

	rendering::c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y, 154 * 2, (items.size() * item_gap), 31, 31, 31, alpha);
	rendering::c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y, 154 * 2, (items.size() * item_gap), 1.f, 255, 40, 40, alpha);

	rendering::c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y + (item_gap * m_current_pos), 154 * 2, item_gap, 26, 26, 26, alpha);
	rendering::c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y + (item_gap * m_current_pos), 154 * 2, item_gap, 1.f, 255, 40, 40, alpha);

	if (items.empty())
		return;
	if (m_current_pos > items.size())
		m_current_pos = items.size();

	if (!items[m_current_pos].tool_tip.empty()) {
		rendering::c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y + (items.size() * item_gap) + 3, 154 * 2, 16, 31, 31, 31, alpha);
		rendering::c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y + (items.size() * item_gap) + 3, 154 * 2, 16, 1.f, 255, 40, 40, alpha);

		rendering::c_renderer::get()->draw_text(this->menu_x, this->menu_y + (items.size() * item_gap) + 4, font_size, items[m_current_pos].tool_tip.c_str(), 255, 255, 255, alpha);

	}

	for (auto item = 0; item < items.size(); ++item) {
		auto height = this->menu_y + (item_gap * item);

		switch (items[item].m_type)
		{
		case item_type::type_bool: {
			rendering::c_renderer::get()->draw_text(this->menu_x, height, font_size, items[item].m_title.c_str(), 255, 255, 255, alpha);
			if (items[item].tab_type == tab_type::type_on_off)
				rendering::c_renderer::get()->draw_text(this->menu_x + 150 + 130, height, font_size, *items[item].m_bool ? "ON" : "OFF", (*items[item].m_bool ? 1 : 255), (*items[item].m_bool ? 255 : 1), 0, alpha);
			else if (items[item].tab_type == tab_type::type_tab)
				rendering::c_renderer::get()->draw_text(this->menu_x + 150 + 138, height - 2, font_size, (*items[item].m_bool ? "<<" : ">>"), 51, 153, 255, alpha);
			break;
		}
		case item_type::type_int: {
			rendering::c_renderer::get()->draw_text(this->menu_x, height, font_size, items[item].m_title.c_str(), 255, 255, 255, alpha);
			rendering::c_renderer::get()->draw_text(this->menu_x + 150 + 130, height, font_size, std::to_string(*items[item].m_int), 255, 255, 255, alpha);
			break;
		}
		case item_type::type_float: {
			rendering::c_renderer::get()->draw_text(this->menu_x, height, font_size, items[item].m_title.c_str(), 255, 255, 255, alpha);
			char buffer[0xFF];
			sprintf_s(buffer, "%.2f", *items[item].m_float);
			rendering::c_renderer::get()->draw_text(this->menu_x + 150 + 130, height, font_size, buffer, 255, 255, 255, alpha);
			break;
		}
		default:
			break;
		}

		if (helpers::get_key_state(VK_UP, 1)) {
			m_current_pos = (m_current_pos > 0) ? (m_current_pos - 1) : items.size() - 1;
		}
		else if (helpers::get_key_state(VK_DOWN, 1)) {
			m_current_pos = (m_current_pos < items.size() - 1) ? (m_current_pos + 1) : 0;
		}
		else if (helpers::get_key_state(VK_LEFT, 1)) {
			switch (items[m_current_pos].m_type) {
			case item_type::type_bool: {
				*items[m_current_pos].m_bool = !*items[m_current_pos].m_bool;
				break;
			}
			case item_type::type_int: {
				*items[m_current_pos].m_int -= items[m_current_pos].m_int_step;
				if (*items[m_current_pos].m_int < items[m_current_pos].m_int_min)
					* items[m_current_pos].m_int = items[m_current_pos].m_int_max;
				break;
			}
			case item_type::type_float: {
				*items[m_current_pos].m_float -= items[m_current_pos].m_float_step;
				if (*items[m_current_pos].m_float < items[m_current_pos].m_float_min)
					* items[m_current_pos].m_float = items[m_current_pos].m_float_max;
				break;
			}
			default:
				break;
			}
		}
		else if (helpers::get_key_state(VK_RIGHT, 1)) {
			switch (items[m_current_pos].m_type) {
			case item_type::type_bool: {
				*items[m_current_pos].m_bool = !*items[m_current_pos].m_bool;
				break;
			}
			case item_type::type_int: {
				*items[m_current_pos].m_int += items[m_current_pos].m_int_step;
				if (*items[m_current_pos].m_int > items[m_current_pos].m_int_max)
					* items[m_current_pos].m_int = items[m_current_pos].m_int_min;
				break;
			}
			case item_type::type_float: {
				*items[m_current_pos].m_float += items[m_current_pos].m_float_step;
				if (*items[m_current_pos].m_float > items[m_current_pos].m_float_max)
					* items[m_current_pos].m_float = items[m_current_pos].m_float_min;
				break;
			}
			default:
				break;
			}
		}
	}
	if (!items.empty())
		items.clear();
}