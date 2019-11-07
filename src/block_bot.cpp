#include "features/features.h"
#include "valve_sdk/csgostructs.hpp"
#include "settings.h"
#include "globals.h"
#include "helpers/input.h"


namespace features
{

	void blockBot(CUserCmd* cmd) //WIP blockbot, not fully working.
	{
		if (!settings::misc::block_bot)
			return;

		if (globals::binds::block_bot == 0 || !input_system::is_key_down(globals::binds::block_bot))
			return;

		c_base_player* localplayer = (c_base_player*)g::entity_list->GetClientEntity(g::engine_client->GetLocalPlayer());

		float bestdist = 250.f;
		int index = -1;

		for (int i = 1; i < g::engine_client->GetMaxClients(); i++)
		{
			c_base_player* entity = (c_base_player*)g::entity_list->GetClientEntity(i);

			if (!entity)
				continue;

			if (!entity->IsAlive() || entity->IsDormant() || entity == localplayer && entity->m_vecVelocity().Length2D() > 0.f)
				continue;

			float dist = localplayer->m_vecOrigin().DistTo(entity->m_vecOrigin());

			if (dist < bestdist)
			{
				bestdist = dist;
				index = i;
			}
		}

		if (index == -1)
			return;

		c_base_player* target = (c_base_player*)g::entity_list->GetClientEntity(index);

		if (!target)
			return;

		QAngle angles = math::CalcAngle(localplayer->m_vecOrigin(), target->m_vecOrigin());

		angles.yaw -= localplayer->m_angEyeAngles().yaw;
		math::AngleNormalize(angles);

		if (angles.yaw < 0.0f)
			cmd->sidemove = 450.f;
		else if (angles.yaw > 0.0f)
			cmd->sidemove = -450.f;
	}

}