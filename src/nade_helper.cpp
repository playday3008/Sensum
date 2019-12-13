#include "nade_helper.h"

nadedata cs[] =
{		//name			//desciption   //wpn   //posx  //posy  //posz //angx  //angy //angz
	{"Connector Smoke", "Jump Throw", "Smoke", 1135.f, 647.f, -197.f, -29.f, -142.f, 0.f},
	{"Jungle Smoke", "Normal", "Smoke", 815.f, -1416.f, -44.f, -27.f, 172.f, 0.f}
};

namespace visuals
{
	void NadeHelper() //Broken, wont draw circle at the specific angle, todo. (WIP)
	{
		Vector pos2D, final;

		for (auto it : cs)
		{
			if (g::debug_overlay->ScreenPosition(Vector(it.posx, it.posy, it.posz), pos2D))
				return;

			VGSHelper::Get().DrawText(it.name, pos2D.x, pos2D.y + 10, Color::White, 15);
			VGSHelper::Get().DrawText(it.description, pos2D.x, pos2D.y + 20, Color::White, 15);
			VGSHelper::Get().DrawText(it.weapon, pos2D.x, pos2D.y + 30, Color::White, 15);

			if (g::debug_overlay->ScreenPosition(Vector(it.anglex, it.angley, it.anglez), final))
				return;

			VGSHelper::Get().DrawCircle(Vector2D(final.x, final.y), 8, 32, Color::White);
		}
	}
}