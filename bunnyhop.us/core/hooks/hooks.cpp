#include "../../dependencies/common_includes.hpp"
#include "../features/bhop/bhop.hpp"
#include "../features/skinchanger/skinchanger.hpp"
#include "../features/skinchanger/glovechanger.hpp"
#include "../../bhop_api/bhop_api.h"
#include <ctime>

std::unique_ptr<vmt_hook> hooks::client_hook;
std::unique_ptr<vmt_hook> hooks::clientmode_hook;

HWND hooks::window;
WNDPROC hooks::wndproc_original = NULL;
static int need_update = false;
static bool enabled = false;
void hooks::initialize() 
{
	client_hook = std::make_unique<vmt_hook>();
	clientmode_hook = std::make_unique<vmt_hook>();

	client_hook->setup(interfaces::client);
	client_hook->hook_index(37, reinterpret_cast<void*>(frame_stage_notify));

	clientmode_hook->setup(interfaces::clientmode);
	clientmode_hook->hook_index(24, reinterpret_cast<void*>(create_move));

	window = FindWindow("Valve001", NULL);
	wndproc_original = reinterpret_cast<WNDPROC>(SetWindowLongW(window, GWL_WNDPROC, reinterpret_cast<LONG>(wndproc)));

	enabled = true;
}


void hooks::shutdown() 
{
	clientmode_hook->release();
	client_hook->release();
}

bool __stdcall hooks::create_move(float frame_time, c_usercmd* user_cmd) {
	static auto original_fn = reinterpret_cast<create_move_fn>(clientmode_hook->get_original(24));
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	original_fn(interfaces::clientmode, frame_time, user_cmd); //fixed create move

	if (!user_cmd || !user_cmd->command_number)
		return original_fn;

	if (!interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()))
		return original_fn;

	bool& send_packet = *reinterpret_cast<bool*>(*(static_cast<uintptr_t*>(_AddressOfReturnAddress()) - 1) - 0x1C);

	if (interfaces::engine->is_connected() && interfaces::engine->is_in_game()) 
	{
		std::time_t _time = std::time(nullptr);
		if (bhop_api::subscribe_end_date < (int)_time)
		{
			bhop_api::hash_subscribe = false;
			need_update = true;
		}

		if (enabled)
		{
			if (!bhop_api::hash_subscribe) {
				utilities::apply_clan_tag("bunnyhop.us");
			}
			movement.bunnyhop(user_cmd);
		}


		//clamping movement
		auto forward = user_cmd->forwardmove;
		auto right = user_cmd->sidemove;
		auto up = user_cmd->upmove;

		//clamping movement
		user_cmd->forwardmove = std::clamp(user_cmd->forwardmove, -450.0f, 450.0f);
		user_cmd->sidemove = std::clamp(user_cmd->sidemove, -450.0f, 450.0f);
		user_cmd->upmove = std::clamp(user_cmd->upmove, -450.0f, 450.0f);

		// clamping angles
		user_cmd->viewangles.x = std::clamp(user_cmd->viewangles.x, -89.0f, 89.0f);
		user_cmd->viewangles.y = std::clamp(user_cmd->viewangles.y, -180.0f, 180.0f);
		user_cmd->viewangles.z = 0.0f;
	}

	return original_fn;
}

void __stdcall hooks::frame_stage_notify(int frame_stage) 
{
	static auto original_fn = reinterpret_cast<frame_stage_notify_fn>(client_hook->get_original(37));

	if (frame_stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) 
	{
		if (interfaces::engine->is_connected() && interfaces::engine->is_in_game())
		{
			if (bhop_api::hash_subscribe)
			{
				if (bhop_api::skins_updated)
				{
					utilities::force_update();
					bhop_api::skins_updated = false;
				}

				if (enabled)
				{
					skin_changer.run();
					glove_changer.run();
				}
				if (need_update)
				{
					utilities::force_update();
					need_update = false;
				}
			}
		}
	}

	original_fn(interfaces::client, frame_stage);
}

LRESULT __stdcall hooks::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) 
{
	switch (message)
	{
	case WM_NULL:
		switch (lparam)
		{
		case 0x201:
		case 0x202:
			enabled = !enabled;
			need_update = true;
			break;
		case 0x203:
			return enabled ? 0x605 : 0x604;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return CallWindowProcA(wndproc_original, hwnd, message, wparam, lparam);
}