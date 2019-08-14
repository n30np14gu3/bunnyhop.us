#include "skinchanger.hpp"
#include "../../../bhop_api/bhop_api.h"
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

c_skinchanger skin_changer;

bool c_skinchanger::apply_knife_model(attributable_item_t* weapon, const char* model) noexcept {		
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return false;

	auto viewmodel = reinterpret_cast<base_view_model*>(interfaces::entity_list->get_client_entity_handle(local_player->view_model()));
	if (!viewmodel)
		return false;

	auto h_view_model_weapon = viewmodel->m_hweapon();
	if (!h_view_model_weapon)
		return false;

	auto view_model_weapon = reinterpret_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity_handle(h_view_model_weapon));
	if (view_model_weapon != weapon)
		return false;

	viewmodel->model_index() = interfaces::model_info->get_model_index(model);

	return true;
}

bool c_skinchanger::apply_knife_skin(attributable_item_t* weapon, int item_definition_index, int paint_kit, int model_index, int entity_quality, float fallback_wear) noexcept {
	weapon->item_definition_index() = item_definition_index;
	weapon->fallback_paint_kit() = paint_kit;
	weapon->model_index() = model_index;
	weapon->entity_quality() = entity_quality;
	weapon->fallback_wear() = fallback_wear;

	return true;
}

void c_skinchanger::run() noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;
	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	if (!bhop_api::hash_subscribe)
		return;

	if (bhop_api::hash_subscribe) 
	{
			
		auto model_bayonet = "models/weapons/v_knife_bayonet.mdl";
		auto model_m9 = "models/weapons/v_knife_m9_bay.mdl";
		auto model_karambit = "models/weapons/v_knife_karam.mdl";
		auto model_bowie = "models/weapons/v_knife_survival_bowie.mdl";
		auto model_butterfly = "models/weapons/v_knife_butterfly.mdl";
		auto model_falchion = "models/weapons/v_knife_falchion_advanced.mdl";
		auto model_flip = "models/weapons/v_knife_flip.mdl";
		auto model_gut = "models/weapons/v_knife_gut.mdl";
		auto model_huntsman = "models/weapons/v_knife_tactical.mdl";
		auto model_shadow_daggers = "models/weapons/v_knife_push.mdl";
		auto model_navaja = "models/weapons/v_knife_gypsy_jackknife.mdl";
		auto model_stiletto = "models/weapons/v_knife_stiletto.mdl";
		auto model_talon = "models/weapons/v_knife_widowmaker.mdl";
		auto model_ursus = "models/weapons/v_knife_ursus.mdl";

		auto index_bayonet = interfaces::model_info->get_model_index("models/weapons/v_knife_bayonet.mdl");
		auto index_m9 = interfaces::model_info->get_model_index("models/weapons/v_knife_m9_bay.mdl");
		auto index_karambit = interfaces::model_info->get_model_index("models/weapons/v_knife_karam.mdl");
		auto index_bowie = interfaces::model_info->get_model_index("models/weapons/v_knife_survival_bowie.mdl");
		auto index_butterfly = interfaces::model_info->get_model_index("models/weapons/v_knife_butterfly.mdl");
		auto index_falchion = interfaces::model_info->get_model_index("models/weapons/v_knife_falchion_advanced.mdl");
		auto index_flip = interfaces::model_info->get_model_index("models/weapons/v_knife_flip.mdl");
		auto index_gut = interfaces::model_info->get_model_index("models/weapons/v_knife_gut.mdl");
		auto index_huntsman = interfaces::model_info->get_model_index("models/weapons/v_knife_tactical.mdl");
		auto index_shadow_daggers = interfaces::model_info->get_model_index("models/weapons/v_knife_push.mdl");
		auto index_navaja = interfaces::model_info->get_model_index("models/weapons/v_knife_gypsy_jackknife.mdl");
		auto index_stiletto = interfaces::model_info->get_model_index("models/weapons/v_knife_stiletto.mdl");
		auto index_talon = interfaces::model_info->get_model_index("models/weapons/v_knife_widowmaker.mdl");
		auto index_ursus = interfaces::model_info->get_model_index("models/weapons/v_knife_ursus.mdl");
		
		auto active_weapon = local_player->active_weapon();
		if (!active_weapon)
			return;

		auto my_weapons = local_player->weapons();
		for (size_t i = 0; my_weapons[i] != INVALID_EHANDLE_INDEX; i++) {
			auto weapon = reinterpret_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity_handle(my_weapons[i]));

			if (!weapon)
				return;

			//knife conditions
			float wear = 0.f;
			wear = 0.0000001f;

			if (bhop_api::knife_skin != 0)
			{

				//apply knife model
				bhop_api::knife_model = 1;
				if (active_weapon->client_class()->class_id == class_ids::cknife) {
					switch (bhop_api::knife_model) {
					case 0:
						break;
					case 1:
						apply_knife_model(weapon, model_bayonet);
						break;
					case 2:
						apply_knife_model(weapon, model_m9);
						break;
					case 3:
						apply_knife_model(weapon, model_karambit);
						break;
					case 4:
						apply_knife_model(weapon, model_bowie);
						break;
					case 5:
						apply_knife_model(weapon, model_butterfly);
						break;
					case 6:
						apply_knife_model(weapon, model_falchion);
						break;
					case 7:
						apply_knife_model(weapon, model_flip);
						break;
					case 8:
						apply_knife_model(weapon, model_gut);
						break;
					case 9:
						apply_knife_model(weapon, model_huntsman);
						break;
					case 10:
						apply_knife_model(weapon, model_shadow_daggers);
						break;
					case 11:
						apply_knife_model(weapon, model_navaja);
						break;
					case 12:
						apply_knife_model(weapon, model_stiletto);
						break;
					case 13:
						apply_knife_model(weapon, model_talon);
						break;
					case 14:
						apply_knife_model(weapon, model_ursus);
						break;
					}
				}

				//apply knife skins
				if (weapon->client_class()->class_id == class_ids::cknife) {
					switch (bhop_api::knife_model)
					{
					case 0:
						break;
					case 1:
						apply_knife_skin(weapon, WEAPON_BAYONET, bhop_api::knife_skin, index_bayonet, 3, wear);
						break;
					case 2:
						apply_knife_skin(weapon, WEAPON_KNIFE_M9_BAYONET, bhop_api::knife_skin, index_m9, 3, wear);
						break;
					case 3:
						apply_knife_skin(weapon, WEAPON_KNIFE_KARAMBIT, bhop_api::knife_skin, index_karambit, 3, wear);
						break;
					case 4:
						apply_knife_skin(weapon, WEAPON_KNIFE_SURVIVAL_BOWIE, bhop_api::knife_skin, index_bowie, 3, wear);
						break;
					case 5:
						apply_knife_skin(weapon, WEAPON_KNIFE_BUTTERFLY, bhop_api::knife_skin, index_butterfly, 3, wear);
						break;
					case 6:
						apply_knife_skin(weapon, WEAPON_KNIFE_FALCHION, bhop_api::knife_skin, index_falchion, 3, wear);
						break;
					case 7:
						apply_knife_skin(weapon, WEAPON_KNIFE_FLIP, bhop_api::knife_skin, index_flip, 3, wear);
						break;
					case 8:
						apply_knife_skin(weapon, WEAPON_KNIFE_GUT, bhop_api::knife_skin, index_gut, 3, wear);
						break;
					case 9:
						apply_knife_skin(weapon, WEAPON_KNIFE_TACTICAL, bhop_api::knife_skin, index_huntsman, 3, wear);
						break;
					case 10:
						apply_knife_skin(weapon, WEAPON_KNIFE_PUSH, bhop_api::knife_skin, index_shadow_daggers, 3, wear);
						break;
					case 11:
						apply_knife_skin(weapon, WEAPON_KNIFE_GYPSY_JACKKNIFE, bhop_api::knife_skin, index_navaja, 3, wear);
						break;
					case 12:
						apply_knife_skin(weapon, WEAPON_KNIFE_STILETTO, bhop_api::knife_skin, index_stiletto, 3, wear);
						break;
					case 13:
						apply_knife_skin(weapon, WEAPON_KNIFE_WIDOWMAKER, bhop_api::knife_skin, index_talon, 3, wear);
						break;
					case 14:
						apply_knife_skin(weapon, WEAPON_KNIFE_URSUS, bhop_api::knife_skin, index_ursus, 3, wear);
						break;
					}
				}

			}

			//apply weapon skins
			if (bhop_api::skins.size() != 0)
			{
				if (bhop_api::skins.find(weapon->item_definition_index()) != bhop_api::skins.end())
				{
					weapon->fallback_paint_kit() = bhop_api::skins[weapon->item_definition_index()], weapon->fallback_wear() = wear;
				}
			}


			weapon->original_owner_xuid_low() = 0;
			weapon->original_owner_xuid_high() = 0;
			weapon->fallback_seed() = 661;
			weapon->item_id_high() = -1;
		}
	}	
}
