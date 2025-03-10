#include "glovechanger.hpp"
#include "../../../bhop_api/bhop_api.h"
c_glovechanger glove_changer;

static auto get_wearable_create_fn() -> create_client_class_fn {
	auto client_class = interfaces::client->get_client_classes();

	for (client_class = interfaces::client->get_client_classes();
		client_class; client_class = client_class->next_ptr){

		if (client_class->class_id == (int)class_ids::ceconwearable){
			return client_class->create_fn;
		}
	}
}

static auto make_glove(int entry, int serial) -> attributable_item_t*{
	static auto create_wearable_fn = get_wearable_create_fn();
	create_wearable_fn(entry, serial);

	const auto glove = static_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity(entry));
	assert(glove);{
		static auto set_abs_origin_addr = utilities::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8") ;
		const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const std::array<float, 3>&)>(set_abs_origin_addr);
		static constexpr std::array<float, 3> new_pos = { 10000.f, 10000.f, 10000.f };
		set_abs_origin_fn(glove, new_pos);
	}
	return glove;
}

bool c_glovechanger::apply_glove_model(attributable_item_t* glove, const char* model) noexcept {
	player_info_t info;
	interfaces::engine->get_player_info(interfaces::engine->get_local_player(), &info);
	glove->acc_id() = info.xuidlow;
	*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;

	return true;
}

bool c_glovechanger::apply_glove_skin(attributable_item_t* glove, int item_definition_index, int paint_kit, int model_index, int entity_quality, float fallback_wear) noexcept {
	glove->item_definition_index() = item_definition_index;
	glove->fallback_paint_kit() = paint_kit;
	glove->set_model_index (model_index);
	glove->entity_quality() = entity_quality;
	glove->fallback_wear() = fallback_wear;

	return true;
}

void c_glovechanger::run() noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;
	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;
	

	// gloveschanger
	if (bhop_api::hash_subscribe) {
		
		if (bhop_api::glove_skin == 0)
			return;

		auto	model_blood = "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl";
		auto	model_sport = "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl";
		auto	model_slick = "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl";
		auto	model_leath = "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl";
		auto	model_moto = "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl";
		auto	model_speci = "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl";
		auto	model_hydra = "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl";

		auto	index_blood = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
		auto	index_sport = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
		auto	index_slick = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
		auto	index_leath = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
		auto	index_moto = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
		auto	index_speci = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
		auto	index_hydra = interfaces::model_info->get_model_index(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"));
		
		//credit to namazso for nskinz
	
		UINT* const wearables = local_player->get_wearables();
		if (!wearables)
			return;

		static UINT glove_handle = UINT(0);
		
		auto glove = reinterpret_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity_handle(wearables[0]));
		
		if (!glove) // There is no glove
		{
			const auto our_glove = reinterpret_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity_handle(glove_handle));

			if (our_glove) // Try to get our last created glove
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}
		if (!local_player || // We are dead but we have a glove, destroy it
			!local_player->is_alive() ||
			!interfaces::engine->is_connected() ||
			!interfaces::engine->is_in_game()
			) {
			if (glove) {
				glove->net_set_destroyed_on_recreate_entities();
				glove->net_release();
			}
			return;
		}
		if (!glove) // We don't have a glove, but we should
		{
			const auto entry = interfaces::entity_list->get_highest_index() + 1;
			const auto serial = rand() % 0x1000;
			glove = make_glove(entry, serial);   // He he
			wearables[0] = entry | serial << 16;
			glove_handle = wearables[0]; // Let's store it in case we somehow lose it.
		}
		if (glove)
		{
			//glove conditions
			float g_wear = 0.f;
			g_wear = 0.0000001f;
			//apply glove model
			switch (bhop_api::glove_model) {
			case 0:
				break;
			case 1:
				apply_glove_model(glove, model_blood);
				break;
			case 2:
				apply_glove_model(glove, model_sport);
				break;
			case 3:
				apply_glove_model(glove, model_slick);
				break;
			case 4:
				apply_glove_model(glove, model_leath);
				break;
			case 5:
				apply_glove_model(glove, model_moto);
				break;
			case 6:
				apply_glove_model(glove, model_speci);
				break;
			case 7:
				apply_glove_model(glove, model_hydra);
				break;
			}
			
			//apply glove skins
			switch (bhop_api::glove_model) {
			case 0:
				break;
			case 1:
				apply_glove_skin(glove, GLOVE_STUDDED_BLOODHOUND, bhop_api::glove_skin, index_blood, 3, g_wear);
				break;
			case 2:
				apply_glove_skin(glove, GLOVE_SPORTY, bhop_api::glove_skin, index_sport, 3, g_wear);
				break;
			case 3:
				apply_glove_skin(glove, GLOVE_SLICK, bhop_api::glove_skin, index_slick, 3, g_wear);
				break;
			case 4:
				apply_glove_skin(glove, GLOVE_LEATHER_WRAP, bhop_api::glove_skin, index_leath, 3, g_wear);
				break;
			case 5:
				apply_glove_skin(glove, GLOVE_MOTORCYCLE, bhop_api::glove_skin, index_moto, 3, g_wear);
				break;
			case 6:
				apply_glove_skin(glove, GLOVE_SPECIALIST, bhop_api::glove_skin, index_speci, 3, g_wear);
				break;
			case 7:
				apply_glove_skin(glove, GLOVE_HYDRA, bhop_api::glove_skin, index_hydra, 3, g_wear);
				break;
			}
						
			glove->item_id_high() = -1;
			glove->fallback_seed() = 0;
			glove->fallback_stattrak() = -1;
									
			glove->net_pre_data_update(DATA_UPDATE_CREATED);
		}
	}
}
