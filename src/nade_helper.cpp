#include "nade_helper.h"

GrenadeData Mirage[] = //Info, Position, Angle
{
	{ "Connector (Jump)", Vector(1135.f, 647.f, -197.f), Vector(-29.42f, -140.f, 0.f), 60.f },
	{ "Jungle (LClick)", Vector(815.f, -1416.f, -44.f), Vector(-28.29f, -173.14f, 0.f), 60.f },
	{ "Window Mid (Jump)", Vector(1423.964355f, -248.026840f, -167.906188f), Vector(-25.723558f, -170.518921f, 0.f), 0.f },
	{ "Catwalk (LClick)", Vector(1423.997803f, 71.985359f, -112.840103f), Vector(-32.653351f, -163.530762f, 0.f), 0.f },
	{ "One-way Mid (RClick)", Vector(369.960144f, -720.522705f, -162.412476f), Vector(35.376301f, 124.135780f, 0.f), 0.f },
	{ "Short (Run)", Vector(399.998444f, 4.656666f, -203.571350f), Vector(-22.060497f, -134.482208f, 0.f), 0.f },
	{ "Window Mid (LClick)", Vector(-624.243530f, 615.992065f, -78.914246f), Vector(-51.595604f, -109.421127f, 0.f), 0.f },
	{ "Kitchen Door (LClick)", Vector(-2325.227783f, 811.800659f, -119.773079f), Vector(-14.883981f, -94.343178f, 0.f), 0.f },
	{ "Short (LClick)", Vector(-160.031250f, 887.968750f, -135.265563f), Vector(-44.269619f, -134.435654f, 0.f), 0.f },
	{ "Kitchen Window (LClick)", Vector(-835.001404f, 615.070190f, -79.065735f), Vector(-63.789238f, -146.581238f, 0.f), 0.f },
	{ "Short (LClick)", Vector(341.836212f, -620.153992f, -163.282486f), Vector(-23.134167f, 164.957458f, 0.f), 0.f },
	{ "Stairs (LClick)", Vector(1148.225586f, -1183.999756f, -205.509155f), Vector(-41.168613f, -165.354019f, 0.f), 0.f },
	{ "Window Mid (Run)", Vector(1391.968750f, -1011.236084f, -167.906188f), Vector(-42.603077f, 172.188919f, 0.f), 0.f },
	{ "Stairs (Jump)", Vector(-90.864479f, -1418.000244f, -115.906189f), Vector(-31.895834f, -62.464306f, 0.f), 0.f },
	{ "Fireboxes (Jump)", Vector(1391.968750f, -930.076294f, -167.906188f), Vector(-21.037338f, -151.204865f, 0.f), 0.f },
	{ "One-way (LClick)", Vector(457.968750f, -1711.996582f, -240.886871f), Vector(-10.477256f, 133.144836f, 0.f), 0.f },
	{ "Bombsite A (LClick)", Vector(815.998718f, -1272.017944f, -108.906189f), Vector(-32.654591f, -149.503601f, 0.f), 0.f },
	{ "CT Spawn (Jump)", Vector(1257.861938f, -871.968750f, -143.906188f), Vector(-21.318205f, -144.344666f, 0.f), 0.f },
	{ "One-way (LClick)", Vector(-1209.077515f, -873.270447f, -167.906188f), Vector(-48.526600f, 67.790833f, 0.f), 0.f },
	{ "One-way (RClick)", Vector(-964.056885f, -2489.520264f, -167.913391f), Vector(-41.926632f, -10.765607f, 0.f), 0.f },
	{ "Ramp (Jump)", Vector(-2026.397583f, -2029.968750f, -299.060150f), Vector(-15.312100f, 12.573707f, 0.f), 0.f },
	{ "One-way Kitchen (LClick)", Vector(-2600.019287f, 535.973022f, -159.906188f), Vector(-16.582365f, -50.818062f, 0.f), 0.f },
	{ "CT Spawn (Jump)", Vector(344.09f, -622.41f, -99.41f), Vector(-20.42f, -116.26f, 0.f), 0.f },
	{ "CT Spawn (LClick)", Vector(-264.03f, -1504.03f, -99.91f), Vector(-20.26f, -108.16f, 0.f), 60.f },
	{ "CT Spawn (LClick)", Vector(-493.03f, -1309.03f, -95.91f), Vector(-6.74f, -108.59f, 0.f), 60.f },
	{ "CT Spawn (LClick)", Vector(-129.52f, -2413.97f, -99.91f), Vector(-2.67f, 166.37f, 0.f), 60.f },
	{ "CT Spawn (LClick)", Vector(-31.92f, -1418.03f, -103.91f), Vector(-35.24f, -116.29f, 0.f), 60.f },
	{ "CT Spawn (Jump)", Vector(1087.09f, -1016.95f, -194.16f), Vector(-29.63f, -144.91f, 0.f), 60.f }

};

GrenadeData Inferno[] = //Info, Position, Angle, Ring Height
{
	{ "CT Spawn (LClick)", Vector(116.294136f, 1580.581055f, 185.121933f), Vector(-32.574593f, 58.334656f, 0.f), 60.f },
	{ "Bombsite B (LClick)", Vector(1971.687988f, 2636.702393f, 128.093811f), Vector(-39.996227f, 175.975357f, 0.f), -10.f },
	{ "Mid (Run)",  Vector(-857.968750f, 449.902283f, -31.604805f),  Vector(-44.401531f, 1.405892f, 0.f), -10.f},
	{ "Coffins (LClick)",  Vector(338.871887f, 1650.802856f, 122.093810f),  Vector(-50.093639f, 84.572739f, 0.f), -10.f },
	{ "B Entrance (LClick)",  Vector(460.464905f, 1828.470825f, 136.182693f),  Vector(-25.443281f, 86.280159f, 0.f), -10.f },
	{ "Long (Run)",  Vector(274.681335f, -224.627777f, 88.093810f),  Vector(-41.052132f, 31.799410f, 0.f), -10.f },
	{ "Pit (LClick)",  Vector(704.160339f, 11.968750f, 88.797089f),  Vector(-52.337799f, -2.135024f, 0.f), -10.f },
	{ "Short (LClick)",  Vector(697.511902f, -242.261810f, 91.093810f),  Vector(-53.097946f, 16.442726f, 0.f), -10.f },
	{ "Library to A (LClick)",  Vector(960.900330f, 434.006409f, 88.093810f),  Vector(-49.863846f, 13.159984f, 0.f), -10.f },
	{ "Pit to Hay (LClick)",  Vector(726.031250f, 220.962921f, 94.029999f),  Vector(-55.241890f, -8.699924f, 0.f), -10.f },
	{ "Long (LClick)",  Vector(491.936829f, -267.968750f, 88.093810f),  Vector(-42.024933f, 45.854645f, 0.f), -10.f },
	{ "Connector (LClick)",  Vector(726.031250f, 288.680084f, 96.093810f),  Vector(-43.560978f, 41.450943f, 0.f), -10.f },
	{ "One-way (LClick)",  Vector(2631.968750f, -16.031250f, 84.093810f),  Vector(-5.692367f, -179.128860f, 0.f), -10.f },
	{ "Balcony (LClick)",  Vector(1913.227295f, 1225.968750f, 174.093811f),  Vector(-46.497322f, -87.005005f, 0.f), 0.f }
};

GrenadeData Dust2[] =
{
	{ "B Entrance (LClick)", Vector(-1846.553223f, 1232.569824f, 32.496025f), Vector(-8.613732f, 118.773392f, 0.f), 0.f },
	{ "CT Mid (LClick)", Vector(-538.606567f, 1382.031250f, -111.957108f), Vector(-35.360550f, 53.845985f, 0.f), 0.f },
	{ "Xbox (LClick)", Vector(229.130554f, 112.497559f, 5.215744f), Vector(-40.624023f, 108.758316f, 0.f), 0.f },
	{ "Long Corner (LClick)", Vector(222.664124f, -87.978210f, 9.093811f), Vector(-46.250572f, 48.164497f, 0.f), 0.f },
	{ "CT Spawn (Run)", Vector(860.031250f, 790.031250f, 3.900847f), Vector(-23.414322f, 43.616291f, 0.f), 0.f },
	{ "Short (LClick)", Vector(489.998535f, 1446.031250f, 2.660506f), Vector(-5.677320f, 88.280685f, 0.f), 0.f },
	{ "Long Gate (LClick)", Vector(1752.049561f, 2046.932739f, 0.365111f), Vector(-33.430305f, -130.546280f, 0.f), 0.f },
	{ "Lower Mid (LClick)", Vector(-242.031250f, 2278.887695f, -119.931587f), Vector(-32.687481f, -123.649094f, 0.f), 0.f },
	{ "Upper Tunnel (LClick)", Vector(-985.452942f, 2553.223877f, 1.318862f), Vector(-26.764244f, -143.848251f, 0.f), 0.f }
};

GrenadeData Nuke[] =
{
	{ "Hangar (LClick)", Vector(-164.092941f, -1954.733765f, -415.916107f), Vector(-13.613587f, 1.278547f, 0.f), 0.f },
	{ "Red Container (LClick)", Vector(-533.003357f, -833.215759f, -193.634827f), Vector(-30.904673f, -43.816589f, 0.f), 0.f },
	{ "Between Containers (Run)", Vector(-423.996399f, -1753.002075f, -415.914856f), Vector(-2.624159f, -50.804165f, 0.f), 0.f },
	{ "T Outside (Jump)", Vector(1664.031250f, -280.002014f, -351.906250f), Vector(-25.048063f, -135.212463f, 0.f), 0.f }
};

GrenadeData Overpass[] =
{
	{ "Toilet Entrance (Run)", Vector(-730.031250f, -2225.143799f, 240.093811f), Vector(-51.612926f, 149.045975f, 0.f), 0.f },
	{ "B Bridge (LClick)", Vector(-617.486389f, -1509.028809f, 144.093811f), Vector(-48.988934f, 113.071342f, 0.f), 0.f },
	{ "B Center (LClick)", Vector(-525.982300f, -1551.984375f, 144.093811f), Vector(-43.807911f, 110.431473f, 0.f), 0.f },
	{ "Barrels to Pillar (LClick)", Vector(-613.014099f, -1082.017212f, 42.160416f), Vector(-29.337307f, 99.340714f, 0.f), 0.f },
	{ "B Center (LClick)", Vector(-1567.968750f, -1087.984619f, 0.093811f), Vector(-30.278185f, 74.646019f, 0.f), 0.f },
	{ "Monster to Pillar (LClick)", Vector(-1645.986938f, -1087.982300f, 96.093810f), Vector(-20.015800f, 55.835869f, 0.f), 0.f },
	{ "Heaven (LClick)", Vector(-1559.968750f, -728.785583f, 52.574355f), Vector(-33.446209f, 96.293686f, 0.f), 0.f },
	{ "B Bridge (LClick)", Vector(-1559.999390f, -361.285339f, 32.421722f), Vector(-43.693123f, 21.193089f, 0.f), 0.f },
	{ "Heaven (LClick)", Vector(-2174.002441f, -1151.968750f, 398.197235f), Vector(-26.368107f, 71.543701f, 0.f), 0.f },
	{ "Long to Boxes (LClick)", Vector(-2025.534058f, -865.001343f, 395.427856f), Vector(-28.313963f, 114.564102f, 0.f), 0.f },
	{ "Bank (LClick)", Vector(-2162.000488f, -519.968750f, 391.460358f), Vector(-29.749702f, 100.836487f, 0.f), 0.f },
	{ "Truck to Bank (LClick)", Vector(-3612.545654f, -177.626740f, 512.791992f), Vector(-40.392601f, 51.259613f, 0.f), 0.f },
	{ "Boxes to Truck (LClick)", Vector(-3540.031250f, -381.968750f, 528.080200f), Vector(-14.256992f, 41.849758f, 0.f), 0.f },
	{ "Truck (LClick)", Vector(-2351.968750f, -815.968750f, 391.089905f), Vector(-34.683971f, 81.500427f, 0.f), 0.f },
	{ "Trash (LClick)", Vector(-2351.968750f, -414.031250f, 388.562317f), Vector(-60.588089f, 73.825958f, 0.f), 0.f },
	{ "Trash (LClick)", Vector(-3383.369629f, 35.247875f, 516.906006f), Vector(-18.035419f, 31.699080f, 0.f), 0.f },
	{ "Long (Duck)", Vector(-1993.398926f, 537.698242f, 475.093810f), Vector(-28.677984f, -169.596695f, 0.f), 0.f },
	{ "Monster (LClick)", Vector(-1926.860962f, 1311.968750f, 355.030579f), Vector(-46.200985f, -40.010532f, 0.f), 0.f },
	{ "One-way (LClick)", Vector(-2191.968750f, 1311.968750f, 356.093811f), Vector(-8.861760f, -55.390415f, 0.f), 0.f },
	{ "One-way (RClick)", Vector(-1826.375610f, 629.178894f, 256.093811f), Vector(26.580435f, -17.457275f, 0.f), 0.f },
	{ "One-way (LClick)", Vector(-2012.968750f, -1231.968750f, 240.093811f), Vector(16.218643f, 63.144173f, 0.f), 0.f },
	{ "Short B (LClick)", Vector(-2115.841309f, 992.920288f, 480.093810f), Vector(-22.936214f, -57.690578f, 0.f), 0.f }
};

GrenadeData Train[] =
{
	{ "B Upper (LClick)", Vector(-1055.968750f, -1607.969116f, -151.906188f), Vector(-9.076089f, -21.028521f, 0.f), 0.f },
	{ "B Lower (LClick)", Vector(-1159.978027f, -1088.112549f, -95.909508f), Vector(-9.122071f, 13.307947f, 0.f), 0.f },
	{ "Blue to Bombsite (Run)", Vector(-1155.979004f, -1301.504395f, -95.906189f), Vector(-15.857571f, 38.882820f, 0.f), 0.f },
	{ "Connector (Run)", Vector(-1159.999634f, -1048.001709f, -95.906189f), Vector(-11.023086f, 5.091055f, 0.f), 0.f },
	{ "Ivy - Right (LClick)", Vector(1388.426270f, 1446.000488f, -223.906189f), Vector(-6.188282f, -95.524574f, 0.f), 0.f },
	{ "Ivy - Left (Run)", Vector(1535.968750f, 1775.968750f, -223.906189f), Vector(-9.818258f, -112.486588f, 0.f), 0.f },
	{ "Bombsite A to Connector (L&R)", Vector(-655.968750f, -399.892731f, 16.093811f), Vector(-46.002502f, 10.890710f, 0.f), 0.f },
	{ "Blue to Red Train (LClick)", Vector(-645.479370f, 1697.721924f, -209.906189f), Vector(-41.564690f, -65.086685f, 0.f), 0.f },
	{ "Electric Box (LClick)", Vector(-481.865631f, 1725.011597f, -209.906189f), Vector(-45.937080f, -78.790627f, 0.f), 0.f },
	{ "Blue Train - Left (LClick)", Vector(-555.031250f, 1262.031250f, -212.532227f), Vector(-68.096550f, -50.974125f, 0.f), 0.f },
	{ "Green to Red Train (LClick)", Vector(-838.162292f, 1268.024414f, -222.906189f), Vector(-37.604507f, -42.064575f, 0.f), 0.f },
	{ "Green Train (LClick)", Vector(-640.027832f, -583.969666f, 16.093811f), Vector(-44.699406f, 32.218452f, 0.f), 0.f },
	{ "Red Train (LClick)", Vector(-453.358459f, 1286.284668f, -86.490753f), Vector(-25.130558f, -58.731426f, 0.f), 0.f },
	{ "Main (LClick)", Vector(1021.096924f, -254.988556f, -215.906189f), Vector(-38.494926f, 154.562332f, 0.f), 0.f }
};

GrenadeData Cache[] =
{
	{ "B Center (LClick)", Vector(827.971313f, -1463.968750f, 1614.093750f), Vector(-21.995483f, 162.191437f, 0.f), 0.f },
	{ "Mid Center (LClick)", Vector(1711.974121f, 463.987732f, 1614.093750f), Vector(-10.675973f, -167.299591f, 0.f), 0.f },
	{ "One-way (LClick)", Vector(1037.031250f, 513.031250f, 1613.550293f), Vector(-49.137814f, 104.639671f, 0.f), 0.f },
	{ "Mid (LClick)", Vector(1711.968750f, -71.968750f, 1614.093750f), Vector(-10.560504f, 161.185349f, 0.f), 0.f },
	{ "Bombsite A (Duck)", Vector(154.413376f, 2096.080566f, 1688.093750f), Vector(9.370919f, -29.337667f, 0.f), 0.f },
	{ "Short (Run)", Vector(139.031250f, 2197.968750f, 1688.093750f), Vector(-6.040052f, -60.836231f, 0.f), 0.f },
	{ "Bombsite A (LClick)", Vector(1319.968750f, 1520.395508f, 1701.093750f), Vector(-57.767025f, 161.424835f, 0.f), 0.f },
	{ "Main (LClick)", Vector(-782.198059f, 1110.000366f, 1689.439697f), Vector(-9.703021f, 24.963852f, 0.f), 0.f },
	{ "Main (LClick)", Vector(-429.968750f, 2244.968750f, 1687.093750f), Vector(-66.017174f, -31.140173f, 0.f), 0.f },
	{ "Main (LClick)", Vector(-50.012558f, 2261.968750f, 1687.093750f), Vector(-18.612713f, -64.612831f, 0.f), 0.f },
	{ "Vents (Run)", Vector(-996.979553f, 1440.231689f, 1691.182373f), Vector(-33.181599f, -46.326721f, 0.f), 0.f },
	{ "CT (Run)", Vector(389.29f, -810.36f, 1678.09f), Vector(-12.47f, 138.20f, 0.f), 0.f },
	{ "Forklift (Run)", Vector(761.97f, 1297.22f, 1766.09f), Vector(-4.16f, 127.43f, 0.f), 60.f }
};

GrenadeData Cobblestone[] =
{
	{ "One-way Long (Duck&RClick)", Vector(272.031250f, -291.031250f, -63.906189f), Vector(-30.971577f, 17.418360f, 0.f), 0.f },
	{ "B Long (Jump)", Vector(-1540.973389f, -1226.978027f, -25.199188f), Vector(-50.672855f, 41.294445f, 0.f), 0.f },
	{ "Matrix (LClick)", Vector(-1864.968750f, -1611.968750f, 96.093810f), Vector(-11.221231f, 136.023499f, 0.f), 0.f },
	{ "B Long (LCLick)", Vector(-288.031250f, 1020.970520f, 128.093811f), Vector(-51.547066f, -53.167721f, 0.f), 0.f },
	{ "Truck - Front (LCLick)", Vector(-3295.975586f, 79.968750f, -29.906188f), Vector(-36.680634f, -52.524323f, 0.f), 0.f },
	{ "Truck - Right (LCLick)", Vector(-3168.031250f, 79.968750f, -29.906188f), Vector(-47.158157f, -65.556221f, 0.f), 0.f },
	{ "Grass (LCLick)", Vector(-3167.270508f, 584.685120f, 0.093811f), Vector(-55.144222f, -61.434193f, 0.f), 0.f },
	{ "Skyfall (LCLick)", Vector(-752.031250f, -80.031250f, 128.093811f), Vector(5.361639f, -119.332336f, 0.f), 0.f },
	{ "Hut - Right (LCLick)", Vector(-155.970673f, -16.010778f, -31.906188f), Vector(-50.869473f, -69.637550f, 0.f), 0.f },
	{ "Hut - Left (LCLick)", Vector(-340.020111f, -80.031250f, -31.907466f), Vector(-53.921837f, -52.166801f, 0.f), 0.f },
	{ "Sandwich (LCLick)", Vector(47.968750f, -16.031250f, -23.114716f), Vector(-81.378204f, -89.289169f, 0.f), 0.f },
	{ "Fountain (LCLick)", Vector(-418.514893f, -95.749924f, -32.562836f), Vector(-75.323563f, -61.343159f, 0.f), 0.f },
	{ "B Door (LCLick)", Vector(-558.031250f, -42.535999f, 0.093811f), Vector(-62.173512f, -100.720726f, 0.f), 0.f },
	{ "Balcony (Jump)", Vector(-2534.005371f, -272.031250f, -184.407272f), Vector(-17.127037f, -65.392319f, 0.f), 0.f },
	{ "A Door (Walk&LClick)", Vector(-3346.178711f, 455.572449f, 0.093811f), Vector(-40.327240f, -45.610413f, 0.f), 0.f },
	{ "A Door (Run)", Vector(-2989.968750f, -944.371948f, 32.093811f), Vector(-12.160514f, -4.402364f, 0.f), 0.f }
};

namespace visuals
{
	Vector CalcHelpPos(Vector target)
	{
		c_base_player* plocal = (c_base_player*)g::entity_list->GetClientEntity(g::engine_client->GetLocalPlayer());

		if (!plocal)
			return Vector(0, 0, 0);

		QAngle vAngle = QAngle(0, 0, 0);

		g::engine_client->GetViewAngles(vAngle);

		float range = 5.f;

		float r_1, r_2;
		float x_1, y_1;

		Vector LocalRendOrig = plocal->GetRenderOrigin();
		Vector LocalViewOfst = plocal->m_vecViewOffset();

		Vector vEyeOrigin = LocalRendOrig + LocalViewOfst;

		r_1 = -(target.y - vEyeOrigin.y);
		r_2 = target.x - vEyeOrigin.x;
		float Yaw = vAngle.yaw - 90.0f;

		float yawToRadian = Yaw * (float)(M_PI / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20.f;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20.f;

		x_1 *= range;
		y_1 *= range;

		return Vector(x_1, y_1, 0);
	}

	static Vector CalcDir(const Vector& vAngles)
	{
		Vector vForward;
		float	sp, sy, cp, cy;

		sy = sin(DEG2RAD(vAngles[1]));
		cy = cos(DEG2RAD(vAngles[1]));

		sp = sin(DEG2RAD(vAngles[0]));
		cp = cos(DEG2RAD(vAngles[0]));

		vForward.x = cp * cy;
		vForward.y = cp * sy;
		vForward.z = -sp;

		return vForward;
	}

	void RenderCircle(ImDrawList* draw_list)
	{
		if (!g::engine_client->IsInGame() || !g::engine_client->IsConnected())
			return;

		if (!settings::misc::smoke_helper)
			return;

		Vector InfoPosScreen;
		Vector CrosshairScreen;

		int x, y;
		g::engine_client->GetScreenSize(x, y);

		int cy = y / 2;
		int cx = x / 2;

		if (strstr(g::engine_client->GetLevelName(), "mirage"))
		{
			for (auto it : Mirage)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "inferno"))
		{
			for (auto it : Inferno)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "dust2"))
		{
			for (auto it : Dust2)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "nuke"))
		{
			for (auto it : Nuke)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "overpass"))
		{
			for (auto it : Overpass)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "train"))
		{
			for (auto it : Train)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "cache"))
		{
			for (auto it : Cache)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "cbble"))
		{
			for (auto it : Cobblestone)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;
				Vector TCircleOfst = CalcHelpPos(it.pos);

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 70 && math::world2screen(Crosshair, CrosshairScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(CrosshairScreen.x - 20.f, CrosshairScreen.y - 10.f), ImVec2(CrosshairScreen.x + size.x + 25.f, CrosshairScreen.y + 12.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircle(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 9.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x, CrosshairScreen.y), 8.f, IM_COL32(255, 255, 0, 255));
						globals::draw_list->AddCircleFilled(ImVec2(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y), 2.f, IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(CrosshairScreen.x + 12.f, CrosshairScreen.y - 7.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
						globals::draw_list->AddLine(ImVec2(cx, cy), ImVec2(CrosshairScreen.x, CrosshairScreen.y), IM_COL32(0, 0, 0, 255), 2.f);
					}
				}
			}
		}
	}

	void DrawRing3D()
	{
		if (!g::engine_client->IsInGame() || !g::engine_client->IsConnected())
			return;

		if (!settings::misc::smoke_helper)
			return;

		if (strstr(g::engine_client->GetLevelName(), "mirage"))
		{
			for (auto it : Mirage)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "inferno"))
		{
			for (auto it : Inferno)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "dust2"))
		{
			for (auto it : Dust2)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "nuke"))
		{
			for (auto it : Nuke)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "overpass"))
		{
			for (auto it : Overpass)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "train"))
		{
			for (auto it : Train)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "cache"))
		{
			for (auto it : Cache)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "cbble"))
		{
			for (auto it : Cobblestone)
			{
				Vector ppos = it.pos;
				Vector Crosshair = g::local_player->GetEyePos() + (CalcDir(it.ang) * 250.f);
				Vector InfoPos = it.pos;
				Vector InfoAng = it.ang;

				auto dist = g::local_player->m_vecOrigin().DistTo(it.pos);

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f)
					{
						VGSHelper::Get().DrawRing3D(ppos.x, ppos.y, ppos.z - it.ring_height, 15.f, 24, Color::White, 1.0f);
					}
				}
			}
		}
	}

	void RenderInfo(ImDrawList* draw_list)
	{
		if (!g::engine_client->IsInGame() || !g::engine_client->IsConnected())
			return;

		if (!settings::misc::smoke_helper)
			return;

		if (strstr(g::engine_client->GetLevelName(), "mirage"))
		{
			for (auto it : Mirage)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "inferno"))
		{
			for (auto it : Inferno)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "dust2"))
		{
			for (auto it : Dust2)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "nuke"))
		{
			for (auto it : Nuke)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "overpass"))
		{
			for (auto it : Overpass)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "train"))
		{
			for (auto it : Train)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "cache"))
		{
			for (auto it : Cache)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}

		if (strstr(g::engine_client->GetLevelName(), "cbble"))
		{
			for (auto it : Cobblestone)
			{
				Vector InfoPosScreen;
				Vector InfoPos = it.pos;

				float dist = g::local_player->m_vecOrigin().DistTo(it.pos);
				auto size = ImGui::CalcTextSize(it.info.c_str());

				if (g::local_player->m_hActiveWeapon())
				{
					if (g::local_player->m_hActiveWeapon()->IsSmoke() && dist <= 500.f && math::world2screen(InfoPos, InfoPosScreen))
					{
						globals::draw_list->AddRectFilled(ImVec2(InfoPosScreen.x - 41.f, InfoPosScreen.y - 75.f), ImVec2(InfoPosScreen.x + size.x - 12.f, InfoPosScreen.y - 60.f), IM_COL32(0, 0, 0, 255));
						globals::draw_list->AddText(ImVec2(InfoPosScreen.x - it.info.length() - 15.f, InfoPosScreen.y - 75.f), IM_COL32(255, 255, 255, 255), it.info.c_str());
					}
				}
			}
		}
	}
}