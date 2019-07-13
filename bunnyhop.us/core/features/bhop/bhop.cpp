#include "../../../dependencies/common_includes.hpp"
#include "bhop.hpp"

c_movement movement;

auto flags_backup = 0;

void c_movement::bunnyhop(c_usercmd* user_cmd) noexcept {
	static bool bLastJumped = false;
	static bool bShouldFake = false;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		user_cmd->buttons |= in_jump;
	}
	else if (user_cmd->buttons & in_jump)
	{
		if (local_player->flags() & fl_onground)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			user_cmd->buttons &= ~in_jump;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void c_movement::autostrafe(c_usercmd* user_cmd) noexcept
{

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player->flags() & fl_onground)
		return;

	if (user_cmd->buttons & in_forward || user_cmd->buttons & in_back || user_cmd->buttons & in_moveleft || user_cmd->buttons & in_moveright)
		return;

	if (user_cmd->mousedx <= 1 && user_cmd->mousedx >= -1)
		return;

	user_cmd->sidemove = user_cmd->mousedx < 0.f ? -450.f : 450.f;
}