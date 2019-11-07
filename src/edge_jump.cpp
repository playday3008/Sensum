#include "features/features.h"
#include "valve_sdk/csgostructs.hpp"
#include "settings.h"
#include "globals.h"
#include "helpers/input.h"

auto flags_backup = 0;

namespace features
{
	void edgeJumpPre(CUserCmd* cmd) noexcept {
		auto local_player = reinterpret_cast<c_base_player*>(interfaces::entity_list->GetClientEntity(interfaces::engine_client->GetLocalPlayer()));

		if (!settings::misc::edge_jump)
			return;

		if (globals::binds::edge_jump == 0 || !input_system::is_key_down(globals::binds::edge_jump))
			return;

		if (!local_player)
			return;

		if (local_player->m_nMoveType() == MOVETYPE_LADDER || local_player->m_nMoveType() == MOVETYPE_NOCLIP)
			return;

		flags_backup = local_player->m_fFlags();
	}

	void edgeJumpPost(CUserCmd* cmd) noexcept {
		auto local_player = reinterpret_cast<c_base_player*>(interfaces::entity_list->GetClientEntity(interfaces::engine_client->GetLocalPlayer()));

		if (!settings::misc::edge_jump)
			return;

		if (globals::binds::edge_jump == 0 || !input_system::is_key_down(globals::binds::edge_jump))
			return;

		if (!local_player)
			return;

		if (local_player->m_nMoveType() == MOVETYPE_LADDER || local_player->m_nMoveType() == MOVETYPE_NOCLIP)
			return;

		if (flags_backup & FL_ONGROUND && !(local_player->m_fFlags() & FL_ONGROUND))
			cmd->buttons |= IN_JUMP;

		if (!(local_player->m_fFlags() & FL_ONGROUND) && settings::misc::edge_jump_duck_in_air)
			cmd->buttons |= IN_DUCK;
	}
}