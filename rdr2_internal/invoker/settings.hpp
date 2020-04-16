#pragma once

struct c_settings
{
public:
	struct {
		bool infinite_stamina = false;
		bool god_mode = false;
		bool semi_godmode = false;
		bool invisible = false;
		bool explode_all = false;
		bool every_ignore = false;
		bool teleport_to_waypoint = false;
		bool trigger_bot = false;
		bool never_wanted = false;
		bool noclip = false;
		bool super_jump = false;
		bool infinite_deadeye = false;
		bool name_changer = false;
		bool model_changer = false;
		bool disable_ragdoll = false;
		int run_speed_multiplier = 0;
		float health_recharge_speed = 1.f;
		float swim_speed = 1.f;
		bool xp_multiplier = false;
	} player;
	struct {
		bool infinite_ammo = false;
		bool explosive_ammo = false;
		bool get_all_weapons = false;
		bool rapid_fire = false;
		bool perfect_accuracy = false;
		float weapon_damage = 1.f;

	} weapon;
	struct {
		bool infinite_stamina = false;
		bool invisible = false;
		bool god_mode = false;
		bool spawn_vehicle = false;
		bool drive_on_water = false;
	} horse;

	struct {
		bool draw_name = false;
		bool draw_distance = false;
		bool draw_box = false;
		bool draw_health = false;
	} esp;

	struct {
		bool unload = false;
	} menu;

	struct {
		int weather = 0;
	} world;

	struct {
		bool spawn_vehicle = false;
		bool spawn_ped = false;
		bool spawn_dead_ped = false;
		bool spawn_object = false;
		bool spawn_ambientpickup = false;
		bool spawn_as_frozen = false;
		bool delete_spawned_models = false;
		bool spawn_gold_chest = false;
	} spawner;

};