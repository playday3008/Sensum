#include "render/render.h"
#include "globals.h"
#include "settings.h"
#include "helpers/imdraw.h"
#include "helpers/console.h"

extern void bind_button(const char* eng, const char* rus, int& key);
extern bool hotkey(const char* label, int* k, const ImVec2& size_arg = ImVec2(0.f, 0.f));

namespace render
{
	namespace menu
	{
		void colors_tab()
		{
			separator("Colors - Chams");
			ColorEdit4("Enemy Visible", &settings::chams::EnemyColor_vis);
			ImGui::SameLine();
			ColorEdit4("Enemy XQZ", &settings::chams::EnemyColor_XQZ);

			ColorEdit4("Team Visible  ", &settings::chams::TeamColor_vis);
			ImGui::SameLine();
			ColorEdit4("Team XQZ", &settings::chams::TeamColor_XQZ);

			ColorEdit4("Local Visible   ", &settings::chams::LocalColor_vis);
			ImGui::SameLine();
			ColorEdit4("Local XQZ", &settings::chams::LocalColor_XQZ);

			separator("Colors - Chams Misc");

			ImGui::ColorEdit4("Viewmodel Weapons", settings::chams::clr_weapon_chams);
			ImGui::ColorEdit4("Planted C4##chams", settings::chams::clr_plantedc4_chams);
			ImGui::ColorEdit4("Dropped Weapons##chams", settings::chams::clr_weapon_dropped_chams);
			ImGui::ColorEdit4("Grenades##chams", settings::chams::clr_nade_chams);

			separator("Colors - ESP");
			ColorEdit4("ESP Visible", &settings::esp::visible_color);
			ImGui::SameLine();
			ColorEdit4("ESP Invisible", &settings::esp::occluded_color);

			ColorEdit4("RCS Cross ", &settings::visuals::recoilcolor);
			ImGui::SameLine();
			ColorEdit4("Spread Cross", &settings::visuals::spread_cross_color);
			ColorEdit4("Aimbot Fov", &settings::visuals::drawfov_color);
			ImGui::SameLine();
			ColorEdit4("Bullet Tracer (?)", &settings::visuals::clr_bullet_tracer);
			tooltip("Disabled saving of Bullet Tracer - Causes bug that will make beams not render :(. Default color is purple.");

			separator("Colors - Glow Players", u8"Руки");

			ImGui::ColorEdit4("Enemy", settings::glow::GlowEnemy);
			ImGui::ColorEdit4("Teammate", settings::glow::GlowTeam);

			separator("Colors - Glow Misc", u8"Руки");

			ImGui::ColorEdit4("Planted C4##glow", settings::glow::GlowC4Planted);
			ImGui::ColorEdit4("Grenades##glow", settings::glow::GlowNades);
			ImGui::ColorEdit4("Dropped Weapons##glow", settings::glow::GlowDroppedWeapons);
		}
	}
}