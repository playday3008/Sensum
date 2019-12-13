#include "../render.h"
#include "../../globals.h"
#include "../../settings.h"
#include "../../helpers/imdraw.h"
#include "../../helpers/console.h"
#include "../..//features/features.h"

extern void bind_button(const char* eng, const char* rus, int& key);
extern bool hotkey(const char* label, int* k, const ImVec2& size_arg = ImVec2(0.f, 0.f));

namespace render
{
	namespace menu
	{
		void visuals_tab()
		{
			child("ESP", []()
			{
				columns(2);
				{
					checkbox("Enabled", u8"Включено", &settings::esp::enabled);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					hotkey("##binds.esp", &globals::binds::esp);
					ImGui::PopItemWidth();
				}
				columns(1);

				checkbox("Visible Only", u8"Проверка видимости", &settings::esp::visible_only);

				checkbox("Name", u8"Имя", &settings::esp::names);

				columns(2);
				{
					checkbox("Weapon", u8"Оружие", &settings::esp::weapons);

					ImGui::NextColumn();

					const char* weapon_modes[] = {
					"Text",
					"Icons"
					};

					ImGui::PushItemWidth(-1);
					{
						ImGui::Combo("Mode", &settings::esp::weapon_mode, weapon_modes, IM_ARRAYSIZE(weapon_modes));
					}
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);

				columns(2);
				{
					checkbox("Player Info Box", &settings::visuals::player_info_box);

					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);
					{
						ImGui::SliderFloatLeftAligned("Alpha##infobox", &settings::visuals::player_info_box_alpha, 0.0f, 1.0f, "%0.1f");
					}
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);

				columns(2);
				{
					checkbox("Grief Box", &settings::visuals::grief_box);

					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);
					{
						ImGui::SliderFloatLeftAligned("Alpha##griefbox", &settings::visuals::grief_box_alpha, 0.0f, 1.0f, "%0.1f");
					}
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);

				columns(2);
				{
					checkbox("Boxes", u8"Боксы", &settings::esp::boxes);

					ImGui::NextColumn();

					const char* box_types[] = {
						___("Normal", u8"Обычные"), ___("Corner", u8"Угловые")
					};

					ImGui::PushItemWidth(-1);
					{
						ImGui::Combo("##esp.box_type", &settings::esp::box_type, box_types, IM_ARRAYSIZE(box_types));
					}
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);

				const char* positions[] =
				{
					___("Left", u8"Слева"),
					___("Right", u8"Справа"),
					___("Bottom", u8"Внизу"),
				};

				const char* HealthPositions[] =
				{
					___("Left", u8"Слева"),
					___("Right", u8"Справа"),
					___("Bottom", u8"Внизу"),
					___("Number", u8"Внизу"),
				};

				columns(2);
				{
					checkbox("Health", u8"Здоровье", &settings::esp::health);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("##health.position", &settings::esp::health_position, HealthPositions, IM_ARRAYSIZE(HealthPositions));
					ImGui::PopItemWidth();
				}
				columns(1);

				columns(2);
				{
					checkbox("Armor", u8"Броня", &settings::esp::armour);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("##armor.position", &settings::esp::armour_position, positions, IM_ARRAYSIZE(positions));
					ImGui::PopItemWidth();
				}
				columns(1);

				//checkbox("Dormant", &Settings::ESP::dormant);
				checkbox("Is Scoped", &settings::esp::is_scoped);
				checkbox("Is Flashed", &settings::esp::is_flashed);
				checkbox("Is Defusing", &settings::esp::is_defusing);
				checkbox("Is Desyncing", &settings::esp::is_desyncing);
				checkbox("Snap Lines", &settings::esp::snaplines);
				checkbox("Ammo ESP", &settings::esp::ammo);
				checkbox("Money ESP", &settings::esp::money);
				checkbox("Choke ESP", &settings::visuals::choke);
				checkbox("Sound ESP", &settings::esp::soundesp);
				checkbox("Buy Log", &settings::esp::buylog);
				//checkbox("Beams", u8"Лучи света", &settings::esp::beams); //Doesnt work.
				//checkbox("Sound Direction (?)", &settings::esp::sound); //Doesnt work.
				//tooltip("Sound ESP", u8"Показывает стрелками направление звука, откуда слышно игрока.");

				checkbox("Bomb Damage ESP", &settings::esp::bomb_esp);
				checkbox("Has Kit ESP", &settings::esp::haskit);

				checkbox("Offscreen ESP", u8"Точка направления (?)", &settings::esp::offscreen);
				checkbox("Enemy Armor Status (?)", &settings::esp::kevlarinfo);
				tooltip("Will display HK if enemy has kevlar + helmer or K if enemy has kevlar only.");
			});

			ImGui::NextColumn();

			child(___("Chams", u8"Цветные Модели"), []()
			{
				static const char* ChamsTypes[] = {
				"Visible - Normal",
				"Visible - Flat",
				"Visible - Wireframe",
				"Visible - Glass",
				"Visible - Metallic",
				"XQZ",
				"Metallic XQZ",
				"Flat XQZ"
				};

				static const char* bttype[] = {
				"Off",
				"Last Tick",
				"All Ticks"
				};

				static const char* chamsMaterials[] = {
				"Normal",
				"Dogtags",
				"Flat",
				"Metallic",
				"Platinum",
				"Glass",
				"Crystal",
				"Gold",
				"Dark Chrome",
				"Plastic/Gloss",
				"Glow"
				};

				columns(2);
				{
					checkbox("Enemy", &settings::chams::enemynew);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("Enemy - Mode", &settings::chams::enemymodenew, ChamsTypes, IM_ARRAYSIZE(ChamsTypes));
					ImGui::PopItemWidth();
				}
				columns(1);

				columns(2);
				{
					checkbox("Team", &settings::chams::teamnew);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("Team - Mode", &settings::chams::teammodenew, ChamsTypes, IM_ARRAYSIZE(ChamsTypes));
					ImGui::PopItemWidth();
				}
				columns(1);

				columns(2);
				{
					checkbox("Local", &settings::chams::localnew);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("Local - Mode", &settings::chams::localmodenew, ChamsTypes, IM_ARRAYSIZE(ChamsTypes));
					ImGui::PopItemWidth();
				}
				columns(1);

				columns(2);
				{
					checkbox("Real Angle   ", &settings::chams::desync);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("Material", &settings::chams::desyncChamsMode, chamsMaterials, IM_ARRAYSIZE(chamsMaterials));
					ImGui::PopItemWidth();
				}
				columns(1);

				//checkbox("Viewmodel Weapons", &settings::chams::wepchams);
				ImGui::SameLine();
				checkbox("Planted C4", &settings::chams::plantedc4_chams);
				checkbox("Weapons (?)       ", &settings::chams::wep_droppedchams);
				tooltip("Dropped Weapons Chams");
				ImGui::SameLine();
				checkbox("Nades", &settings::chams::nade_chams);
				checkbox("Health Chams", &settings::chams::health_chams);

				//separator("BT Chams - Mode");
				//ImGui::Combo("BT Chams Mode", &settings::chams::bttype, bttype, IM_ARRAYSIZE(bttype));
				//checkbox("BT Chams - Flat", &settings::chams::btflat);
				//ColorEdit4("BT Color", &settings::chams::btColorChams);

				/*separator("Arms", u8"Руки");

				checkbox("Enabled##arms", u8"Включено##arms", &settings::chams::arms::enabled);
				checkbox("Wireframe##arms", u8"Сетка##arms", &settings::chams::arms::wireframe);

				ImGui::Separator();

				ColorEdit4(___("Visible", u8"Видимый"), &settings::chams::visible_color);
				ColorEdit4(___("Occluded", u8"За преградой"), &settings::chams::occluded_color);

				ColorEdit4(___("Arms", u8"Руки"), &settings::chams::arms::color); */

				child(___("Glow", u8"Цветные Модели"), []()
				{
					checkbox("Enemy", &settings::glow::glowEnemyEnabled);
					ImGui::SameLine();
					checkbox("Planted C4", &settings::glow::glowC4PlantedEnabled);
					ImGui::SameLine();
					checkbox("Nades", &settings::glow::glowNadesEnabled);
					checkbox("Team  ", &settings::glow::glowTeamEnabled);
					ImGui::SameLine();
					checkbox("Weapons (?)", &settings::glow::glowDroppedWeaponsEnabled);
					tooltip("Dropped Weapons Glow");
				});
			});

			ImGui::NextColumn();

			child(___("Extra", u8"Прочее"), []()
			{
				static const char* cross_types[] = {
					"Type: Crosshair",
					"Type: Circle"
				};

				static const char* hitmarkersounds[] = {
					"Sound: Cod",
					"Sound: Skeet",
					"Sound: Punch",
					"Sound: Metal",
					"Sound: Boom"
				};

				checkbox("Planted C4", &settings::visuals::planted_c4);
				checkbox("Defuse Kits", u8"Дефуза", &settings::visuals::defuse_kit);
				checkbox("World Weapons", u8"Подсветка оружий", &settings::visuals::dropped_weapons);
				checkbox("World Grenades", u8"Подсветка гранат", &settings::visuals::world_grenades);
				checkbox("Sniper Crosshair", u8"Снайперский прицел", &settings::visuals::sniper_crosshair);
				checkbox("Grenade Prediction", u8"Прогноз полета гранат", &settings::visuals::grenade_prediction);
				checkbox("Damage Indicator", &settings::misc::damage_indicator);
				checkbox("Aimbot Fov", &settings::esp::drawFov);
				checkbox("Spread Crosshair", &settings::visuals::spread_cross);
				checkbox("Bullet Tracer", &settings::visuals::bullet_tracer);
				checkbox("Hitmarker", &settings::visuals::hitmarker);
				ImGui::Combo("Hitmarker Sound", &settings::visuals::hitsound, hitmarkersounds, IM_ARRAYSIZE(hitmarkersounds));
				checkbox("RCS Crosshair", &settings::visuals::rcs_cross);
				ImGui::Combo("RCS Crosshair Type", &settings::visuals::rcs_cross_mode, cross_types, IM_ARRAYSIZE(cross_types));
				if (settings::visuals::rcs_cross_mode == 1)
					ImGui::SliderFloatLeftAligned("Radius", &settings::visuals::radius, 8.f, 18.f, "%.1f");

				const auto old_night_state = settings::visuals::night_mode;
				const auto old_style_state = settings::visuals::newstyle;
				checkbox("Night Mode", u8"Ночной режим", &settings::visuals::night_mode);

				if (settings::visuals::night_mode)
				{
					ImGui::SliderFloatLeftAligned(___("Night Mode Intensity:", u8"Радиус:"), &settings::esp::mfts, 0.0f, 1.0f, "%.1f %");

					if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
					{
						color_modulation::SetMatForce();
					}
				}

				checkbox("Chance ESP (?)", &settings::misc::esp_random);
				tooltip("Enables/disables the esp/chams based on chance, that is generated per round.Set chance manually.");

				if (settings::misc::esp_random)
				{
					ImGui::SliderIntLeftAligned("ESP Chance (?)", &settings::esp::esp_chance, 1, 100, "%.0f %%");
					tooltip("Will turn esp/chams on/off if chance is higher/smaller or equal than set value");
				}

				checkbox("Dark Menu", &settings::visuals::newstyle);
				if (old_style_state != settings::visuals::newstyle) //settings::visuals::night_mode
					imdraw::apply_style(settings::visuals::newstyle);
			});
		}
	}
}