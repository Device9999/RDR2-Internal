#pragma once
#include "../main/rdr2_main.hpp"

namespace features
{
	class c_features
	{
	public:

		void godmodes(Ped player_ped_id, Player player_id);

		void infinite_staminas(Ped player_ped_id);

		void explode_all(Ped player_ped_id);

		void infinite_ammo(Ped player_ped_id);

		void change_player_model(Hash model, Ped player_ped_id, Player player_id);

		void get_all_weapons(Ped player_ped_id);

		void teleport_to_waypoint(Ped player_ped_id);

		void spawn_ped(std::string model_name, bool as_dead, Ped player_ped_id);

		void spawn_object(Hash model, Ped player_ped_id);

		void spawn_vehicle(std::string model_name, Ped player_ped_id);

		void noclip(Ped player_ped_id);

		void on_tick();

	};
}