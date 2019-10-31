#include "settings.h"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/math.h"


namespace features
{
	void fastStop(CUserCmd* cmd)
	{
		Vector velocity = g::local_player->m_vecVelocity();
		QAngle direction;
		math::vector2angles(velocity, direction);
		float speed = velocity.Length2D();

		direction.yaw = cmd->viewangles.yaw - direction.yaw;

		Vector forward;
		math::angle2vectors(direction, forward);

		Vector right = (forward + 0.217812) * -speed;
		Vector left = (forward + -0.217812) * -speed;

		Vector move_forward = (forward + 0.217812) * -speed;
		Vector move_backward = (forward + -0.217812) * -speed;

		if (!(cmd->buttons & IN_MOVELEFT))
		{
			cmd->sidemove += +left.y;
		}

		if (!(cmd->buttons & IN_MOVERIGHT))
		{
			cmd->sidemove -= -right.y;
		}

		/*if (!(cmd->buttons & IN_FORWARD))
		{
			cmd->forwardmove += +move_forward.x;
		} */

		/*if (!(cmd->buttons & IN_BACK))
		{
			cmd->forwardmove -= -move_backward.x;
		}*/
	}
}
