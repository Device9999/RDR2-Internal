#include "features.hpp"
#include "../menu/menu.hpp"
#include <array>

namespace features 
{
	void c_features::godmodes(Ped player_ped_id, Player player_id) {
		static bool o_god = settings.player.god_mode;
		if (o_god != settings.player.god_mode) {
			PLAYER::SET_PLAYER_INVINCIBLE(player_id, !o_god);
			ENTITY::SET_ENTITY_INVINCIBLE(player_ped_id, !o_god);
			o_god = settings.player.god_mode;
		}

		static bool o_hgod = settings.horse.god_mode;
		if (o_hgod != settings.horse.god_mode) {
			if (PED::IS_PED_ON_MOUNT(player_ped_id)) {
				auto horse = PED::GET_MOUNT(player_ped_id);
				ENTITY::SET_ENTITY_INVINCIBLE(horse, !o_hgod);
			}
			o_hgod = settings.horse.god_mode;
		}
	}

	void c_features::infinite_staminas(Ped player_ped_id) {
		if (settings.horse.infinite_stamina) {
			if (PED::IS_PED_ON_MOUNT(player_ped_id)) {
				Ped horse = PED::GET_MOUNT(player_ped_id);
				PED::SET_PED_STAMINA(horse, 100.f);
				//ATTRIBUTE::_0xC6258F41D86676E0(horse, 0, 100);
				//ATTRIBUTE::_0xC6258F41D86676E0(horse, 1, 100);
				//ATTRIBUTE::_0xC6258F41D86676E0(horse, 2, 100);

			}
		}
		if (settings.player.infinite_stamina) {
			PED::SET_PED_STAMINA(player_ped_id, 100.f);
			ATTRIBUTE::_0xC6258F41D86676E0(player_ped_id, 1, 100);
			PLAYER::RESTORE_PLAYER_STAMINA(PLAYER::PLAYER_ID(), 1.0);
		}

	}

	void c_features::explode_all(Ped player_ped_id) {

		for (int i = 0; i < 32; i++) {
			auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i);
			if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped) || ped == player_ped_id)
				continue;
			auto position = ENTITY::GET_ENTITY_COORDS(ped, false, false);
			if (position.x == 0.f && position.y == 0.f && position.z == 0.f)
				continue;

			FIRE::ADD_OWNED_EXPLOSION(player_ped_id, position.x, position.y, position.z, 0, 0.5f, true, false, 0.0f);
		}
	}


	void c_features::infinite_ammo(Ped player_ped_id) {
		Hash current = 0;
		static auto semiauto = MISC::GET_HASH_KEY("WEAPON_PISTOL_SEMIAUTO"), // these doesnt seem to get picked up by GET_CURRENT_PED_WEAPON
			mauser = MISC::GET_HASH_KEY("WEAPON_PISTOL_MAUSER"),
			tomahawk_ancient = MISC::GET_HASH_KEY("WEAPON_THROWN_TOMAHAWK_ANCIENT"),
			tomahawk = MISC::GET_HASH_KEY("WEAPON_THROWN_TOMAHAWK"),
			cleaver = MISC::GET_HASH_KEY("WEAPON_MELEE_CLEAVER"),
			hatchet = MISC::GET_HASH_KEY("WEAPON_MELEE_HATCHET"),
			bow = MISC::GET_HASH_KEY("WEAPON_BOW");

		if (WEAPON::GET_CURRENT_PED_WEAPON(player_ped_id, &current, 0, 0, 0) && WEAPON::IS_WEAPON_VALID(current))
		{
			static int max_ammo = 9999;
			if (WEAPON::GET_MAX_AMMO(player_ped_id, &max_ammo, current))
				WEAPON::SET_PED_AMMO(player_ped_id, current, max_ammo);
			max_ammo = WEAPON::GET_MAX_AMMO_IN_CLIP(player_ped_id, current, 1);
			if (max_ammo > 0)
				WEAPON::SET_AMMO_IN_CLIP(player_ped_id, current, max_ammo);

				WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, current);
		}
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, semiauto);
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, mauser);
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, tomahawk_ancient);
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, tomahawk);
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, cleaver);
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, hatchet);
		WEAPON::SET_PED_INFINITE_AMMO(player_ped_id, TRUE, bow);

	}

	void c_features::change_player_model(Hash model, Ped player_ped_id, Player player_id) {

		if (STREAMING::IS_MODEL_IN_CDIMAGE(model))
		{
			STREAMING::REQUEST_MODEL(model, 1);
			int tries = 0;
			while (!STREAMING::HAS_MODEL_LOADED(model) && tries < 25) {
				fiber::wait_for(0);
				tries++;
			}
			if (STREAMING::HAS_MODEL_LOADED(model)) {
				PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model, 1);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			}
		}

	}

	void c_features::get_all_weapons(Ped player_ped_id) { // doesn't work
		auto give_weapon = [](Ped ped, Hash weapName) {
			return invoke<Void>(0x5E3BDDBCB83F3D84, ped, weapName, 60, true, 1, false, 0.0);
		};

		auto give_ammo = [](Ped ped, Hash weapHash) {
			invoke<Void>(0x106A811C6D3035F3, ped, weapHash, 100);
		};

		for (auto weapon : weapons_names) {
			if (!weapon.empty()) {
				auto hash = MISC::GET_HASH_KEY(weapon.c_str());
				give_weapon(player_ped_id, hash);
				give_ammo(player_ped_id, hash);
			}
		}
	}

	void c_features::teleport_to_waypoint(Ped player_ped_id) {
		const auto set_ground_coords = [](Ped ent_to_tele, Vector3 coords, bool vehicle, int tries) {
			float z_coords = 0.0f;
			float ground_heights[11] = { 10.0f, 50.0f, 100.0f, 175.0f, 225.0f, 350.0f, 500.0f, 1000.0f, 1500.0f, -50.0f, -10.f };
			for (auto i = 0; i < tries; ++i)
			{
				if (MISC::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, coords.z, &z_coords, 10000.0f) || z_coords != 0.f)
					break;
				fiber::wait_for(150);
				coords.z = ground_heights[i];

				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent_to_tele, coords.x, coords.y, coords.z + 0.6f, 0, 0, 0);
				if (vehicle)
					VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(ent_to_tele, 1.f);
			}
			if (z_coords == 0.f)
				z_coords = 750.f;

			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent_to_tele, coords.x, coords.y, z_coords + 0.6f, 0, 0, 0);
			if (vehicle)
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(ent_to_tele, 1.f);
		};

		auto waypoint_coords = MAP::_GET_WAYPOINT_COORDS();
		if (waypoint_coords.is_valid_xy()) {
			auto is_veh = false;
			auto ent_to_tele = player_ped_id;
			if (PED::IS_PED_ON_MOUNT(player_ped_id)) // is ped on horse?
				ent_to_tele = PED::GET_MOUNT(player_ped_id);
			else if (PED::IS_PED_IN_ANY_VEHICLE(player_ped_id, 0)) {
				ent_to_tele = PED::GET_VEHICLE_PED_IS_USING(player_ped_id);
				is_veh = true;
			}

			set_ground_coords(ent_to_tele, Vector3(waypoint_coords.x, waypoint_coords.y, 0.f), is_veh, 10);
			//auto coords = get_ground_coords(Vector3(waypoint_coords.x, waypoint_coords.y, 0.f), 10);
			//ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, coords.x, coords.y, coords.z, 0, 0, 0);
		}
	}

	void c_features::spawn_ped(std::string model_name, bool as_dead, Ped player_ped_id) {
		DWORD model = MISC::GET_HASH_KEY(model_name.c_str()); // A_C_BUCK_01
		if (STREAMING::IS_MODEL_IN_CDIMAGE(model)) {
			STREAMING::REQUEST_MODEL(model, 1);
			int tries = 0;
			while (!STREAMING::HAS_MODEL_LOADED(model) && tries < 25) {
				fiber::wait_for(0);
				tries += 1;
			}
			if (!STREAMING::HAS_MODEL_LOADED(model))
				return;

			float forward = 5.f;
			float heading = ENTITY::GET_ENTITY_HEADING(player_ped_id);
			float x = forward * sin(DEG2RAD(heading)) * -1.f;
			float y = forward * cos(DEG2RAD(heading));

			Vector3 coords = ENTITY::GET_ENTITY_COORDS(player_ped_id, 0, 0);
			uint32_t ped_ = invoke<uint32_t, uint32_t, float, float, float, float, bool, bool, bool, bool, bool, bool>(0xD49F9B0955C367DE, model, coords.x + x, coords.y + y, coords.z, heading, false, false, false, false, true, true);
		
			hooks::globals::delete_entities.emplace_back(ped_);

			if (settings.spawner.spawn_as_frozen)
				ENTITY::FREEZE_ENTITY_POSITION(ped_, true);

			NETWORK::NETWORK_REGISTER_ENTITY_AS_NETWORKED(ped_);
			NETWORK::_NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK(ped_, false);
			DWORD id = NETWORK::PED_TO_NET(ped_);
			if (NETWORK::NETWORK_DOES_NETWORK_ID_EXIST(id))	{
				ENTITY::_SET_ENTITY_SOMETHING(ped_, true);
				if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(ped_)) {
					NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(id, true);
					NETWORK::_NETWORK_CAN_NETWORK_ID_BE_SEEN(id);
				}
			}

			if (as_dead)
				PED::APPLY_DAMAGE_TO_PED(ped_, 400, false);
			ENTITY::SET_ENTITY_VISIBLE(ped_, 1);
			ENTITY::SET_ENTITY_ALPHA(ped_, 255, 0);
			PED::SET_PED_VISIBLE(ped_, true);
			//PED::SET_PED_MAX_HEALTH(ped_, 0);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			//printf("SPAWNED PED\n");
		}
	}

	void c_features::spawn_object(Hash model, Ped player_ped_id) {
		STREAMING::REQUEST_MODEL(model, 1);
		int tries = 0;
		while (!STREAMING::HAS_MODEL_LOADED(model) && tries < 25) {
			fiber::wait_for(0);
			tries += 1;
		}
		if (!STREAMING::HAS_MODEL_LOADED(model))
			return;

		float forward = 5.f;
		float heading = ENTITY::GET_ENTITY_HEADING(player_ped_id);
		float x = forward * sin(DEG2RAD(heading)) * -1.f;
		float y = forward * cos(DEG2RAD(heading));

		Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player_ped_id, 0.0, 5.0, 0.0);
		auto object = OBJECT::CREATE_OBJECT(model, coords.x + x, coords.y + y, coords.z, 1, 1, 1);
		hooks::globals::delete_entities.emplace_back(object);

		if (settings.spawner.spawn_as_frozen)
			ENTITY::FREEZE_ENTITY_POSITION(object, true);

		NETWORK::NETWORK_REGISTER_ENTITY_AS_NETWORKED(object);
		NETWORK::_NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK(object, false);
		DWORD id = NETWORK::OBJ_TO_NET(object);
		if (NETWORK::NETWORK_DOES_NETWORK_ID_EXIST(id)) {
			ENTITY::_SET_ENTITY_SOMETHING(object, true);
			if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(object)) {
				NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(id, true);
				NETWORK::_NETWORK_CAN_NETWORK_ID_BE_SEEN(id);
			}
		}
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
	}

	void c_features::spawn_vehicle(std::string model_name, Ped player_ped_id) {
		DWORD model = MISC::GET_HASH_KEY(model_name.c_str());
		if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_A_VEHICLE(model))
		{
			STREAMING::REQUEST_MODEL(model, 1);
			int tries = 0;
			while (!STREAMING::HAS_MODEL_LOADED(model) && tries < 25) {
				fiber::wait_for(0);
				tries += 1;
			}
			if (!STREAMING::HAS_MODEL_LOADED(model))
				return;

			float forward = 5.f;
			float heading = ENTITY::GET_ENTITY_HEADING(player_ped_id);
			float x = forward * sin(DEG2RAD(heading)) * -1.f;
			float y = forward * cos(DEG2RAD(heading));

			Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player_ped_id, 0.0, 5.0, 0.0);
			Vehicle veh = VEHICLE::CREATE_VEHICLE(model, coords.x + x, coords.y + y, coords.z, heading, 1, 1);

			hooks::globals::delete_entities.emplace_back(veh);

			if (settings.spawner.spawn_as_frozen)
				ENTITY::FREEZE_ENTITY_POSITION(veh, true);

			NETWORK::NETWORK_REGISTER_ENTITY_AS_NETWORKED(veh);
			DECORATOR::DECOR_SET_BOOL(veh, "wagon_block_honor", true);
			VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh, 1.f);
			NETWORK::_NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK(veh, false);
			DWORD id = NETWORK::VEH_TO_NET(veh);

			if (NETWORK::NETWORK_DOES_NETWORK_ID_EXIST(id))
			{
				ENTITY::_SET_ENTITY_SOMETHING(veh, true);
				if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh)) {
					NETWORK::_NETWORK_CAN_NETWORK_ID_BE_SEEN(veh);
					NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(id, true);
				}
			}

			ENTITY::SET_ENTITY_HEADING(veh, ENTITY::GET_ENTITY_HEADING(player_ped_id));
			PED::SET_PED_INTO_VEHICLE(player_ped_id, veh, -1);


			fiber::wait_for(0);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
		}

	}

	void c_features::noclip(Ped entity) {
		if (PED::IS_PED_ON_MOUNT(entity))
			entity = PED::GET_MOUNT(entity);
		else if (PED::IS_PED_IN_ANY_VEHICLE(entity, 0))
			entity = PED::GET_VEHICLE_PED_IS_USING(entity);


		auto pos = ENTITY::GET_ENTITY_COORDS(entity, false, false);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, pos.x, pos.y, pos.z, false, false, false);
		float heading = ENTITY::GET_ENTITY_HEADING(entity);
		float m = 1.5f;

		if (GetAsyncKeyState('S')) {
			float xVec = m * sin(DEG2RAD(heading)) * -1.0f;
			float yVec = m * cos(DEG2RAD(heading));
			ENTITY::SET_ENTITY_HEADING(entity, heading);

			pos.x -= xVec, pos.y -= yVec;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, pos.x, pos.y, pos.z, false, false, false);
		}
		if (GetAsyncKeyState('W')) {
			float xVec = m * sin(DEG2RAD(heading)) * -1.0f;
			float yVec = m * cos(DEG2RAD(heading));
			ENTITY::SET_ENTITY_HEADING(entity, heading);

			pos.x += xVec, pos.y += yVec;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, pos.x, pos.y, pos.z, false, false, false);
		}
		if (GetAsyncKeyState('A')) {
			ENTITY::SET_ENTITY_HEADING(entity, heading + 0.8f);
		}
		if (GetAsyncKeyState('D')) {
			ENTITY::SET_ENTITY_HEADING(entity, heading - 0.8f);
		}
		if (GetAsyncKeyState(VK_SHIFT)) {
			ENTITY::SET_ENTITY_HEADING(entity, heading);

			pos.z -= 1.0f;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, pos.x, pos.y, pos.z, false, false, false);
		}
		if (GetAsyncKeyState(VK_SPACE)) {
			ENTITY::SET_ENTITY_HEADING(entity, heading);
			pos.z += 1.0f;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, pos.x, pos.y, pos.z, false, false, false);
		}
	}


	void c_features::on_tick() {
		auto player_ped_id = PLAYER::PLAYER_PED_ID();
		auto player_id = PLAYER::PLAYER_ID();
		
		if (!ENTITY::DOES_ENTITY_EXIST(player_ped_id) || !NETWORK::NETWORK_IS_PLAYER_CONNECTED(player_id))
			return;
		auto local_head_pos = PED::GET_PED_BONE_COORDS(player_ped_id, 0x796e, 0.f, 0.f, 0.f);

		if (settings.player.trigger_bot) {
			Entity aim_ent = 0; 
			if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player_id, &aim_ent) && ENTITY::DOES_ENTITY_EXIST(aim_ent) && (ENTITY::GET_ENTITY_HEALTH(aim_ent) > 0) && ENTITY::IS_ENTITY_A_PED(aim_ent)) {

				//auto position = PED::GET_PED_BONE_COORDS(aim_ent, 0, 0.0f, 0.0f, 0.0f);
				auto position = PED::GET_PED_BONE_COORDS(aim_ent, 0x796e, 0.0f, 0.0f, 0.0f);//ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(aim_ent, PED::GET_PED_BONE_INDEX(aim_ent, 0x796e));
				if (position.x != 0.f && position.y != 0.f && position.z != 0.f) {
					Hash current = 0;
					if (WEAPON::GET_CURRENT_PED_WEAPON(player_ped_id, &current, 0, 0, 0) && WEAPON::IS_WEAPON_VALID(current)) {
						if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
							MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(local_head_pos.x, local_head_pos.y, local_head_pos.z, position.x, position.y, position.z, 100, 1, current, player_ped_id, 1, 0, 9999.f);
					}
				}
			}
		}

		if (settings.spawner.spawn_vehicle) {
			static std::string ped_;
			if (ped_.empty()) {
				ped_ = helpers::get_keyboard_input("Input model name of vehicle to be spawned, model_name-amount.");
			}
			else {
				static std::string amount;
				auto pos = ped_.find("-");
				if (pos != std::string::npos) {
					amount = ped_.substr(pos + 1);
					ped_.erase(pos, ped_.size());
				}
				else {
					amount = "1";
				}

				auto max_it = std::stoi(amount.c_str());
				for (int i = 0; i < max_it; i++)
					spawn_vehicle(ped_.c_str(), player_ped_id);

				ped_.clear();
				settings.spawner.spawn_vehicle = false;
			}
		}

		if (settings.spawner.spawn_ped) {
			static std::string ped_;
			if (ped_.empty()) {
				ped_ = helpers::get_keyboard_input("Input model name of ped to be spawned, model_name-amount.");
			}
			else {
				static std::string amount;
				auto pos = ped_.find("-");
				if (pos != std::string::npos) {
					amount = ped_.substr(pos + 1);
					//printf("Amount %s\n", amount.c_str());
					ped_.erase(pos, ped_.size());
					//printf("ped_ %s\n", ped_.c_str());
				}
				else {
					amount = "1";
				}

				auto max_it = std::stoi(amount.c_str());
				for (int i = 0; i < max_it; i++)
					spawn_ped(ped_.c_str(), false, player_ped_id);

				ped_.clear();
				settings.spawner.spawn_ped = false;
			}
		}

		if (settings.spawner.spawn_dead_ped) {
			static std::string ped_;
			if (ped_.empty()) {
				ped_ = helpers::get_keyboard_input("Input model name of dead ped to be spawned, model_name-amount.");
			}
			else {
				static std::string amount;
				auto pos = ped_.find("-");
				if (pos != std::string::npos) {
					amount = ped_.substr(pos + 1);
					ped_.erase(pos, ped_.size());
				} else {
					amount = "1";
				}

				auto max_it = std::stoi(amount.c_str());
				for (int i = 0; i < max_it; i++)
					spawn_ped(ped_.c_str(), true, player_ped_id);

				ped_.clear();
				settings.spawner.spawn_dead_ped = false;
			}
		}

		if (settings.spawner.spawn_object) {
			static std::string ped_;
			if (ped_.empty()) {
				ped_ = helpers::get_keyboard_input("Input model name of object to be spawned, model_name-amount.");
			}
			else {
				static std::string amount;
				auto pos = ped_.find("-");
				if (pos != std::string::npos) {
					amount = ped_.substr(pos + 1);
					//printf("Amount %s\n", amount.c_str());
					ped_.erase(pos, ped_.size());
					//printf("ped_ %s\n", ped_.c_str());
				}
				else {
					amount = "1";
				}

				auto max_it = std::stoi(amount.c_str());
				for (int i = 0; i < max_it; i++)
					spawn_object(MISC::GET_HASH_KEY(ped_.c_str()), player_ped_id);

				ped_.clear();
				settings.spawner.spawn_object = false;
			}
		}

		if (settings.spawner.spawn_ambientpickup) {
			static std::string model_name;
			if (model_name.empty()) {
				model_name = helpers::get_keyboard_input("Input model name of pickup to be spawned, model_name-pickup_name.");
			}
			else {
				static std::string pickupname;
				auto pos = model_name.find("-");
				if (pos != std::string::npos) {
					pickupname = model_name.substr(pos + 1);
					model_name.erase(pos, model_name.size());
				}
				else {
					pickupname = "1";
				}

				auto hash_of_model = MISC::GET_HASH_KEY(model_name.c_str());

				auto origin = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0, 0, 0);
				float forward = 5.f;
				float heading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
				float x = forward * sin(DEG2RAD(heading)) * -1.f;
				float y = forward * cos(DEG2RAD(heading));

				STREAMING::REQUEST_MODEL(hash_of_model, 1);
				int tries = 0;
				while (!STREAMING::HAS_MODEL_LOADED(hash_of_model) && tries < 25) {
					fiber::wait_for(0);
					tries += 1;
				}

				if (STREAMING::HAS_MODEL_LOADED(hash_of_model)) {
					auto pickup = OBJECT::CREATE_AMBIENT_PICKUP(MISC::GET_HASH_KEY(pickupname.c_str()), (origin.x + x), (origin.y + y), origin.z + 0.5f, 0, 2000, hash_of_model, FALSE, TRUE);
					NETWORK::_NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK(pickup, false);
				}

				model_name.clear();
				settings.spawner.spawn_ambientpickup = false;
			}
		}

		if (settings.player.name_changer) {

			static std::string new_name;
			if (new_name.empty()) {
				new_name = helpers::get_keyboard_input("Input the name you want yours to be changed to.");
			}
			else {
				const char* buffer = new_name.c_str();
				const size_t length = new_name.size() + 1;
				//static std::vector<uintptr_t> rdr2_offsets = {
				//			0x3D3F3B2, 0x514163C, 0x5776224,
				//			0x5782F10, 0x5783058, 0x578AF20,
				//			0x578B068, 0x578FD14, 0x5794C07 
				//};				
				static std::vector<uintptr_t> rdr2_offsets = {
					0x3D91B92, 0x519311C, 0x57D08D4,
					0x57DA0FF, 0x57DA4C4, 0x57DD560,
					0x57DD6A8, 0x57E5570, 0x57E56B8,
					0x57EA364, 0x57EF2E7
				};

				//DWORD sc_ptrs[] = { 0x2FFD5C, 0x302D2F, 0x305D98, 0x305EE0, 0x30EA78, 0x30EBC0 };
				for (auto offset : rdr2_offsets){
					auto ptr = reinterpret_cast<char*>(hooks::globals::base_address + offset);
					if (ptr)
						strncpy(ptr, buffer, length);
				}
				static auto sc_base = (uintptr_t)(GetModuleHandleA("socialclub.dll"));
				auto ptr = reinterpret_cast<char*>(sc_base + 0x84903C);
				if (ptr) {
					strncpy(ptr, buffer, length);
					auto ptr = reinterpret_cast<char*>(sc_base + 0x84D44F);
					strncpy(ptr, buffer, length);
				}

				new_name.clear();
				settings.player.name_changer = false;
			}
		}

		if (settings.player.run_speed_multiplier > 0) {
			auto ent = PED::IS_PED_ON_MOUNT(player_ped_id) ? PED::GET_MOUNT(player_ped_id) :  player_ped_id;
			if ((TASK::IS_PED_RUNNING(ent) || TASK::IS_PED_SPRINTING(ent)) && !PED::IS_PED_RUNNING_RAGDOLL_TASK(ent)) {
				ENTITY::APPLY_FORCE_TO_ENTITY(ent, true, 0, settings.player.run_speed_multiplier, 0, 0, 0, 0, true, true, true, true, false, true);
			}
		}

		if (settings.weapon.rapid_fire) {
			PED::SET_PED_SHOOT_RATE(player_ped_id, 999.f);
		}

		static auto o_ragd = settings.player.disable_ragdoll;
		if (o_ragd != settings.player.disable_ragdoll) {
			o_ragd = settings.player.disable_ragdoll;
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::PLAYER_PED_ID(), !o_ragd);
			PED::SET_PED_CAN_RAGDOLL(player_ped_id, !o_ragd);
		}
		if (settings.weapon.perfect_accuracy) {
			PED::SET_PED_ACCURACY(player_ped_id, 0);
		}

		if (settings.weapon.explosive_ammo) {
			vector_3_aligned bullet_coords = vector_3_aligned();
			static auto o_bullet_coords = bullet_coords;
			if (PED::IS_PED_SHOOTING(player_ped_id) && WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(player_ped_id, &bullet_coords)) {
				if (bullet_coords != o_bullet_coords) {
					FIRE::ADD_OWNED_EXPLOSION(player_ped_id, bullet_coords.x, bullet_coords.y, bullet_coords.z, 0, 0.5f, false, false, 0.0f);
					o_bullet_coords = bullet_coords;
				}
			}
		}

		if (settings.player.model_changer) {
			static std::string new_model_name;
			if (new_model_name.empty()) {
				new_model_name = helpers::get_keyboard_input("Input the name you want yours to be changed to.");
			}
			else {
				this->change_player_model(MISC::GET_HASH_KEY(new_model_name.c_str()), player_ped_id, player_id);

				new_model_name.clear();
				settings.player.model_changer = false;
			}
		}


		static auto o_wdamage = settings.weapon.weapon_damage;
		if (o_wdamage != settings.weapon.weapon_damage) {
			o_wdamage = settings.weapon.weapon_damage;
			PLAYER::SET_PLAYER_WEAPON_DAMAGE_MODIFIER(player_id, o_wdamage);
			PLAYER::SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(player_id, o_wdamage);
		}

		static auto o_recharge = settings.player.health_recharge_speed;
		if (o_recharge != settings.player.health_recharge_speed) {
			o_recharge = settings.player.health_recharge_speed;
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(player_id, o_recharge);
		}

		static auto o_swimspd = settings.player.swim_speed;
		if (o_swimspd != settings.player.swim_speed) {
			o_swimspd = settings.player.swim_speed;
			PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(player_id, o_swimspd);
		}


		if (settings.spawner.spawn_gold_chest) {
			DWORD chest_hash = -1587197023;
			DWORD reward_hash = 716341297;
			Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0f, 0.8f, -0.75f);
			float heading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
			if (STREAMING::IS_MODEL_IN_CDIMAGE(chest_hash) && STREAMING::IS_MODEL_VALID(chest_hash)) {

				STREAMING::REQUEST_MODEL(chest_hash, 0);
				STREAMING::REQUEST_MODEL(reward_hash, 0);

				float playerHeading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
				auto gold_object = OBJECT::CREATE_OBJECT(chest_hash, coords.x, coords.y, coords.z, 1, 1, 1);
				if (ENTITY::DOES_ENTITY_EXIST(gold_object)) {
					hooks::globals::delete_entities.emplace_back(gold_object);

					helpers::request_control_of_ent(gold_object);
					ENTITY::SET_ENTITY_HEADING(gold_object, heading);
					ENTITY::SET_ENTITY_ALPHA(gold_object, 255, 0);
					ENTITY::SET_ENTITY_VISIBLE(gold_object, 1);
					NETWORK::NETWORK_REGISTER_ENTITY_AS_NETWORKED(gold_object);
					auto netID = NETWORK::OBJ_TO_NET(gold_object);
					NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(netID, 1);

				}
				for (int i = 0; i < 50; i++) {
					gold_object = OBJECT::CREATE_OBJECT(reward_hash, coords.x, coords.y, coords.z, 1, 1, 1);
					if (ENTITY::DOES_ENTITY_EXIST(gold_object)) {
						hooks::globals::delete_entities.emplace_back(gold_object);
						helpers::request_control_of_ent(gold_object);
						ENTITY::SET_ENTITY_HEADING(gold_object, heading);
						ENTITY::SET_ENTITY_ALPHA(gold_object, 255, 0);
						ENTITY::SET_ENTITY_VISIBLE(gold_object, 1);
						NETWORK::NETWORK_REGISTER_ENTITY_AS_NETWORKED(gold_object);
						auto netID = NETWORK::OBJ_TO_NET(gold_object);
						NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(netID, 1);

						fiber::wait_for(10);
					}
				}
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(chest_hash);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(reward_hash);
			}
			settings.spawner.spawn_gold_chest = false;
		}

		if (settings.player.semi_godmode) {
			auto max_health = ENTITY::GET_ENTITY_MAX_HEALTH(player_ped_id, FALSE);
			auto health = ENTITY::GET_ENTITY_HEALTH(player_ped_id);

			if (health < max_health)
				ENTITY::SET_ENTITY_HEALTH(player_ped_id, max_health, FALSE);

			ATTRIBUTE::_0xC6258F41D86676E0(player_ped_id, 0, 100);
		}

		static auto o_ignore = settings.player.every_ignore;
		if (o_ignore != settings.player.every_ignore) {
			PLAYER::SET_EVERYONE_IGNORE_PLAYER(player_id, !o_ignore);
			o_ignore = settings.player.every_ignore;
		}

		if (settings.player.infinite_deadeye) {
			PLAYER::RESTORE_SPECIAL_ABILITY(player_id, -1, FALSE);
			ATTRIBUTE::_0xC6258F41D86676E0(player_ped_id, 2, 100);
		}

		if (settings.player.super_jump) {
			MISC::SET_SUPER_JUMP_THIS_FRAME(player_id);
		}

		if (settings.player.never_wanted) {
			PURSUIT::CLEAR_CURRENT_PURSUIT();
			PURSUIT::SET_PLAYER_PRICE_ON_A_HEAD(player_id, 0);
			PURSUIT::SET_PLAYER_WANTED_INTENSITY(player_id, 0);
			PLAYER::SET_WANTED_LEVEL_MULTIPLIER(0.0f);
		}

		static auto o_inv = false;
		if (settings.player.invisible) {
			ENTITY::SET_ENTITY_VISIBLE(player_ped_id, 0);
			ENTITY::SET_ENTITY_ALPHA(player_ped_id, 0, 0);
			PED::SET_PED_VISIBLE(player_ped_id, false);
			//NETWORK::_NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK(player_ped_id, true);
			o_inv = true;
		}
		else if (o_inv) {
			ENTITY::SET_ENTITY_VISIBLE(player_ped_id, 1);
			ENTITY::SET_ENTITY_ALPHA(player_ped_id, 254, 0);
			PED::SET_PED_VISIBLE(player_ped_id, true);
			o_inv = false;
		}

		static auto o_hinv = false;
		if (settings.horse.invisible) {
			if (PED::IS_PED_ON_MOUNT(player_ped_id)) {
				auto entity = PED::GET_MOUNT(player_ped_id);
				//NETWORK::_NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK(player_ped_id, false);
				ENTITY::SET_ENTITY_VISIBLE(entity, 0);
				ENTITY::SET_ENTITY_ALPHA(entity, 0, 0);
				PED::SET_PED_VISIBLE(entity, false);
			}
			o_hinv = true;
		}
		else if (o_hinv) {
			if (PED::IS_PED_ON_MOUNT(player_ped_id)) {
				auto entity = PED::GET_MOUNT(player_ped_id);
				ENTITY::SET_ENTITY_VISIBLE(entity, 1);
				ENTITY::SET_ENTITY_ALPHA(entity, 255, 0);
				PED::SET_PED_VISIBLE(entity, true);
				o_hinv = false;
			}
		}

		if (settings.player.noclip)
			this->noclip(player_ped_id);


		this->godmodes(player_ped_id, player_id);

		this->infinite_staminas(player_ped_id);

		if (settings.weapon.infinite_ammo)
			this->infinite_ammo(player_ped_id);

		if (settings.weapon.get_all_weapons) {
			get_all_weapons(player_ped_id);
			settings.weapon.get_all_weapons = false;
		}

		/*
		\xE8\x00\x00\x00\x00\x48\x8B\x5C\x24\x00\x48\x83\xC4\x20\x5F\xC3\xCC\x48\x2B\x83\xA1\xE0\x3E\x00, x????xxxx?xxxxxxxxxxxxxx
		static  void* (*set_lobby_weather)(char a1, int a2, int a3, __int64 a4) = reinterpret_cast<decltype(set_lobby_weather)>((PVOID)(hooks::globals::base_address + 0x23D50B0));
		set_lobby_weather = reinterpret_cast<decltype(set_lobby_weather)>((PVOID)(hooks::globals::base_address + 0x23D50B0));
		*/

		if (settings.player.teleport_to_waypoint) {
			this->teleport_to_waypoint(player_ped_id);
			settings.player.teleport_to_waypoint = false;
		}

		if (settings.player.explode_all) {
			this->explode_all(player_ped_id);
			settings.player.explode_all = false;
		}

		struct player_options_t
		{
			bool tab_open = false;
			bool explode = false;
			bool teleport_to = false;
			bool clone = false;
			//bool teleport_to_me = false;
			bool freeze = false;
			bool spawn_ped = false;
			bool spawn_dead_ped = false;
			bool spawn_object = false;
			bool spawn_vehicle = false;

		};



		static bool players_tab = false;
		static std::array<player_options_t, 33> players;
		menu_framework->add_entry("PLAYERS", &players_tab, true);
		if (players_tab) {
			menu_framework->add_entry("Explode All Players", &settings.player.explode_all, 0, "Explodes all players expect you.");
			//menu_framework->add_entry("RETURN", &players_tab, true);
			if (settings.esp.draw_name || settings.esp.draw_distance)
				settings.esp.draw_name = settings.esp.draw_distance = false;

			static std::array<bool, 33> was_open;
			for (int i = 0; i < 32; i++) {
				auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i);
				if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped) || ped == player_ped_id)
					continue;
				auto position = ENTITY::GET_ENTITY_COORDS(ped, false, false);
				if (!position.is_valid_xyz())
					continue;
				auto name = PLAYER::GET_PLAYER_NAME(i);
				auto health = ENTITY::GET_ENTITY_HEALTH(ped);

				menu_framework->add_entry((std::string("  ") + name + " | " + std::to_string(health) + "hp | " + std::to_string(int(position.dist_to(local_head_pos))) + "m"), &players[i].tab_open, 1);

				if (players[i].tab_open && !was_open[i]) {
					for (int j = 0; j < 32; j++) {
						if (j != i)
							players[j].tab_open = false;
					}
				}
				was_open[i] = players[i].tab_open;

				auto player = players[i];
				if (player.tab_open) {
					menu_framework->add_entry("  Explode", &players[i].explode, 0);
					menu_framework->add_entry("  Teleport to", &players[i].teleport_to, 0);
					//menu_framework->add_entry("  Teleport to me", &players[i].teleport_to_me, 0);
					menu_framework->add_entry("  Freeze", &players[i].freeze, 0);
					menu_framework->add_entry("  Clone", &players[i].clone, 0);

					menu_framework->add_entry("  Spawn Vehicle", &players[i].spawn_vehicle, 0);
					menu_framework->add_entry("  Spawn Object", &players[i].spawn_object, 0);
					menu_framework->add_entry("  Spawn Ped", &players[i].spawn_ped, 0);
					menu_framework->add_entry("  Spawn Dead Ped", &players[i].spawn_dead_ped, 0);
					static bool passed_once_veh = false; // retarded bandaid fix
					if (players[i].spawn_vehicle) {
						if (passed_once_veh) {
							players[i].spawn_vehicle = false;
							passed_once_veh = false;
							goto end_veh;
						}
						static std::string ped_;
						if (ped_.empty()) {
							ped_ = helpers::get_keyboard_input("Input model name of vehicle to be spawned, model_name-amount.");
						}
						else {
							passed_once_veh = true;
							static std::string amount;
							auto pos = ped_.find("-");
							if (pos != std::string::npos) {
								amount = ped_.substr(pos + 1);
								ped_.erase(pos, ped_.size());
							}
							else {
								amount = "1";
							}

							auto max_it = std::stoi(amount.c_str());
							for (int i = 0; i < max_it; i++)
								spawn_vehicle(ped_.c_str(), ped);

							ped_.clear();
							players[i].spawn_vehicle = false;
						}
					}
					end_veh:
					static bool passed_once_ped = false; // retarded bandaid fix
					if (players[i].spawn_ped) {
						if (passed_once_ped) {
							players[i].spawn_ped = false;
							passed_once_ped = false;
							goto end_ped;
						}
						static std::string ped_;
						if (ped_.empty()) {
							ped_ = helpers::get_keyboard_input("Input model name of ped to be spawned, model_name-amount.");
						}
						else {
							passed_once_ped = true;
							static std::string amount;
							auto pos = ped_.find("-");
							if (pos != std::string::npos) {
								amount = ped_.substr(pos + 1);
								ped_.erase(pos, ped_.size());
							}
							else {
								amount = "1";
							}

							auto max_it = std::stoi(amount.c_str());
							for (int i = 0; i < max_it; i++)
								spawn_ped(ped_.c_str(), false, ped);

							ped_.clear();
							settings.spawner.spawn_ped = false;
						}
					}
				end_ped:
					static bool passed_once_dped = false; // retarded bandaid fix
					if (players[i].spawn_dead_ped) {
						if (passed_once_dped) {
							players[i].spawn_dead_ped = false;
							passed_once_dped = false;
							goto end_dped;
						}
						static std::string ped_;
						if (ped_.empty()) {
							ped_ = helpers::get_keyboard_input("Input model name of dead ped to be spawned, model_name-amount.");
						}
						else {
							passed_once_dped = true;
							static std::string amount;
							auto pos = ped_.find("-");
							if (pos != std::string::npos) {
								amount = ped_.substr(pos + 1);
								ped_.erase(pos, ped_.size());
							}
							else {
								amount = "1";
							}

							auto max_it = std::stoi(amount.c_str());
							for (int i = 0; i < max_it; i++)
								spawn_ped(ped_.c_str(), true, ped);

							ped_.clear();
							players[i].spawn_dead_ped = false;
						}
					}
				end_dped:

					static bool passed_once_obj = false; // retarded bandaid fix
					if (players[i].spawn_object) {
						if (passed_once_obj) {
							players[i].spawn_object = false;
							passed_once_obj = false;
							goto end_obj;
						}
						static std::string ped_;
						if (ped_.empty() && !passed_once_obj) {
							ped_ = helpers::get_keyboard_input("Input model name of object to be spawned, model_name-amount.");
						}
						else {
							passed_once_obj = true;
							static std::string amount;
							auto pos = ped_.find("-");
							if (pos != std::string::npos) {
								amount = ped_.substr(pos + 1);
								ped_.erase(pos, ped_.size());
							}
							else {
								amount = "1";
							}

							auto max_it = std::stoi(amount.c_str());
							for (int i = 0; i < max_it; i++)
								spawn_object(MISC::GET_HASH_KEY(ped_.c_str()), ped);

							ped_.clear();
							players[i].spawn_object = false;
						}
					}
				end_obj:

					if (players[i].explode) {
						FIRE::ADD_OWNED_EXPLOSION(player_ped_id, position.x, position.y, position.z, 0, 0.5f, true, false, 0.0f);
						players[i].explode = false;
					}

					if (players[i].teleport_to) {
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player_ped_id, position.x, position.y, position.z, 0, 0, 0);
						players[i].teleport_to = false;
					}

					if (players[i].clone) {
						PED::CLONE_PED(ped, ENTITY::GET_ENTITY_HEADING(ped), 0, 1);
						players[i].clone = false;
					}

					if (players[i].freeze) {
						TASK::CLEAR_PED_TASKS_IMMEDIATELY(ped, 1, 1);
						TASK::CLEAR_PED_SECONDARY_TASK(ped);
					}
				}
			}
		}


		if (settings.spawner.delete_spawned_models && hooks::globals::draw_delete_option && !hooks::globals::delete_entities.empty()) {
			for (int i = 0; i < hooks::globals::delete_entities.size(); i++) {
				auto ent = hooks::globals::delete_entities[i];
				ENTITY::DELETE_ENTITY(&ent);

				hooks::globals::delete_entities.erase(hooks::globals::delete_entities.begin() + i);
			}
		}
		if (settings.spawner.delete_spawned_models && hooks::globals::delete_entities.empty())
			settings.spawner.delete_spawned_models = false;
	}
}