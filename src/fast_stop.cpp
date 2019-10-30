#include "settings.h"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/math.h"


namespace features
{
	void fastStop(CUserCmd* cmd)
	{
		const auto velocity = g::local_player->m_vecVelocity();
		QAngle direction;
		math::vector2angles(velocity, direction);
		float speed = velocity.Length2D();

		direction.yaw = cmd->viewangles.yaw - direction.yaw;

		Vector forward;
		math::angle2vectors(direction, forward);

		Vector source = forward * -speed;

		if (!(cmd->buttons & IN_MOVELEFT))
		{
			cmd->sidemove += +source.y;
		}

		if (!(cmd->buttons & IN_MOVERIGHT))
		{
			cmd->sidemove -= -source.y;
		}

		/*if (!(cmd->buttons & IN_FORWARD))
		{
			cmd->forwardmove += +source.x;
		} */

		/*if (!(cmd->buttons & IN_BACK))
		{
			cmd->forwardmove -= -source.x;
		}*/
	}
}
