#include "../render.h"
#include "../../globals.h"
#include "../../settings.h"
#include "../../features/features.h"
#include "../../helpers/console.h"

extern void bind_button(const char* eng, const char* rus, int& key);
extern bool hotkey(const char* label, int* k, const ImVec2& size_arg = ImVec2(0.f, 0.f));

namespace render
{
	namespace menu
	{
		char name[64];
		char localtag[16];
		bool is_clantag_copied = false;

		void misc_tab()
		{
			child(___("Name", u8"Имя"), []()
			{
				ImGui::InputText("##misc_name", name, sizeof(name));
				columns(2);
				{
					if (ImGui::Button(___("Apply##name", u8"Применить##name"), ImVec2(ImGui::GetContentRegionAvailWidth() - 2.f, 0.f)))
						utils::set_player_name(name);

					ImGui::NextColumn();

					if (ImGui::Button(___("Hide", u8"Скрыть"), ImVec2(ImGui::GetContentRegionAvailWidth() - 2.f, 0.f)))
						utils::set_player_name("\n\xAD\xAD\xAD");
				}
				columns(1);

				separator(___("Clan Tag", u8"Клантег"));

				if (!is_clantag_copied && std::string(localtag) != globals::clantag::value)
				{
					is_clantag_copied = true;
					strcpy(localtag, globals::clantag::value.c_str());
				}

				ImGui::InputText("##misc_clantag", localtag, sizeof(localtag));
				//if (globals::clantag::animation)
					//ImGui::SliderFloatLeftAligned(___("Delay:", u8"Задержка:"), &globals::clantag::delay, 0, 1);

				auto player_resource = *interfaces::player_resource;
				if (player_resource)
				{
					if (ImGui::BeginCombo("##clantags.list", ___("Player tags", u8"Теги игроков")))
					{
						std::vector<std::string> tags;

						for (int i = 1; i < interfaces::engine_client->GetMaxClients(); ++i)
						{
							auto* player = c_base_player::GetPlayerByIndex(i);
							if (!player)
								continue;

							const auto info = player->GetPlayerInfo();
							if (info.fakeplayer)
								continue;

							auto usertag = std::string(player_resource->GetClanTag()[player->GetIndex()]);
							if (usertag.empty() || std::find(tags.begin(), tags.end(), usertag) != tags.end())
								continue;

							tags.push_back(usertag);

							if (ImGui::Selectable(usertag.c_str()))
							{
								strcpy(localtag, usertag.c_str());
								globals::clantag::value = usertag;
								clantag::set(localtag);
							}
						}

						ImGui::EndCombo();
					}
				}

				columns(2);
				{
					if (ImGui::Button(___("Apply##clan", u8"Применить##clan"), ImVec2(ImGui::GetContentRegionAvailWidth() - 2.f, 0.f)))
					{
						globals::clantag::value = localtag;
						clantag::set(localtag);

						globals::save();
					}

					ImGui::NextColumn();

					if (ImGui::Button(!globals::clantag::animation ? ___("Animation", u8"Аним.") : ___("Dont Animate", u8"Не аним."), ImVec2(ImGui::GetContentRegionAvailWidth() - 2.f, 0.f)))
						globals::clantag::animation = !globals::clantag::animation;
				}
				columns(1);

				ImGui::PushID("fakelags");
				{
					columns(2);
					{
						checkbox("Enabled", u8"Включено", &settings::fake_lags::enabled);

						ImGui::NextColumn();

						ImGui::PushItemWidth(-1);
						hotkey("##binds.fake_lags", &globals::binds::fake_lag);
						ImGui::PopItemWidth();
					}
					columns(1);

					const char* lag_types[] =
					{
						___("Always", u8"Всегда"),
						___("Only in air", u8"Только в прыжке"),
						___("When picking", u8"Во время пика"),
						___("By button", u8"По кнопке"),
						___("Adaptive", u8"По кнопке")
					};

					ImGui::Combo("##lag_type", &settings::fake_lags::type, lag_types, IM_ARRAYSIZE(lag_types));
					ImGui::SliderIntLeftAligned(___("Factor:", u8"Фактор:"), &settings::fake_lags::factor, 1, 6, settings::fake_lags::factor == 1 ? "%.0f tick" : "%.0f ticks");
				}
				ImGui::PopID();

				static const char* desyncModes[] = {
					"Off",
					"Static",
					"Balance"
				};

				separator("Desync", u8"Десинхронизация");
				ImGui::PushID("desync");
				{
					columns(2);
					{
						checkbox("Enabled", u8"Включено", &settings::desync::enabled2);

						ImGui::NextColumn();
						ImGui::Combo("Mode", &settings::desync::desync_mode, desyncModes, IM_ARRAYSIZE(desyncModes));
					}
					columns(1);

					bind_button("Left/Right", u8"Left/Right", globals::binds::desync);
				}
				ImGui::PopID();

				separator("FOV");

				ImGui::SliderFloatLeftAligned(___("View Model:", u8"Руки:"), &settings::misc::viewmodel_fov, 54, 120, "%.0f *");
				ImGui::SliderIntLeftAligned(___("Debug:", u8"Обзор:"), &settings::misc::debug_fov, 80, 120, "%.0f *");

				static const char* skyList[] = { "Baggage", "Tibet", "Embassy", "Italy", "Daylight 1", "Daylight 2", "Daylight 3", "Daylight 4", "Cloudy", "Night 1", "Night 2", "Night Flat", "Day HD", "Day", "Rural", "Vertigo HD", "Vertigo", "Dusty Sky", "Jungle", "Nuke", "Office" };

				separator("Skybox Changer");

				columns(2);
				{
					checkbox("Enabled", &settings::visuals::skychanger);

					ImGui::NextColumn();
					ImGui::Combo("List", &settings::visuals::skychanger_mode, skyList, IM_ARRAYSIZE(skyList));
				}
				columns(1);

				if (settings::visuals::skychanger)
				{
					color_modulation::SkyChanger();
				}

				static const char* viewList[] = {
					"X",
					"Y",
					"Z"
				};

				separator("Viewmodel Override");

				columns(2);
				{
					checkbox("Enabled##viewmodel_changer", &settings::misc::override_viewmodel);

					ImGui::NextColumn();
					ImGui::Combo("List##viewmodel_changer", &settings::visuals::viewmodel_mode, viewList, IM_ARRAYSIZE(viewList));
				}
				columns(1);

				switch (settings::visuals::viewmodel_mode)
				{
				case 0:
					ImGui::SliderFloatLeftAligned("Offset X:", &settings::misc::viewmodel_offset_x, -21.f, 21.f, "%.0f");
					break;
				case 1:
					ImGui::SliderFloatLeftAligned("Offset Y:", &settings::misc::viewmodel_offset_y, 0.f, 50.f, "%.0f");
					break;
				case 2:
					ImGui::SliderFloatLeftAligned("Offset Z:", &settings::misc::viewmodel_offset_z, -30.f, 30.f, "%.0f");
					break;
				}

				/*if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
				  {
						color_modulation::SkyChanger();
				  } */
			});

			ImGui::NextColumn();

			child(___("Extra", u8"Прочее"), []()
			{
				//checkbox("Russian", u8"Русский язык", &globals::russian_language);
				//checkbox("Disable Animations (?)", u8"Отключить анимации (?)", &globals::no_animations);
				//tooltip("Disables the cheat menu animations.", u8"Отключает анимации меню чита.");

				const char* fastStopModes[] = {
					"Off",
					"Left & Right",
					"Forward & Backward",
					"Both"
				};

				checkbox("Engine Prediction", &settings::movement::engine_prediction);
				checkbox("Radar", u8"Радар", &settings::misc::radar);
				checkbox("No Flash", u8"Убрать световые", &settings::misc::no_flash);
				checkbox("No Smoke", u8"Убрать дымовые", &settings::misc::no_smoke);
				checkbox("Bunny Hop", u8"Распрыжка", &settings::misc::bhop);
				checkbox("Auto Strafe", u8"Стрейфы", &settings::misc::auto_strafe);
				checkbox("Knife Bot", u8"Ножевой бот", &settings::misc::knife_bot);
				checkbox("Moon Walk", u8"Лунная походка", &settings::misc::moon_walk);
				checkbox("Deathmatch", u8"Бой насмерть", &settings::misc::deathmatch);
				checkbox("Post Processing", u8"Постообработка", &globals::post_processing);
				checkbox("Resolver", u8"Постообработка", &settings::desync::resolver);
				checkbox("Humanised Bhop", u8"Постообработка", &settings::misc::human_bhop);
				checkbox("Noscope Overlay", u8"Постообработка", &settings::misc::noscope);
				checkbox("No 3rd Person on Equiped Weapons.", &settings::misc::disable_on_weapon);
				checkbox("Left Hand Knife", &settings::misc::lefthandknife);
				checkbox("-98 Nade (?)", &settings::misc::selfnade);
				tooltip("Look up, Hold mouse 2,When you're fully primed to throw with mouse 2 start holding mouse 1, crouch when released.");
				columns(2);
				{
					checkbox("Fast Stop", &settings::misc::fast_stop);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					ImGui::Combo("Mode", &settings::misc::fast_stop_mode, fastStopModes, IM_ARRAYSIZE(fastStopModes));
					ImGui::PopItemWidth();
				}
				columns(1);

				checkbox("Force Inventory Open", &settings::misc::force_inventory_open);

				columns(2);
				{
					checkbox("Edge Jump", &settings::misc::edge_jump);

					ImGui::NextColumn();

					ImGui::PushItemWidth(-1);
					if (settings::misc::edge_jump) {
						ImGui::Checkbox("Duck in Air", &settings::misc::edge_jump_duck_in_air);
					}
					ImGui::PopItemWidth();
				}
				columns(1);
				//checkbox("Block Bot", &settings::misc::block_bot); //WIP blockbot, not fully working
			});

			ImGui::NextColumn();

			child(___("Binds", u8"Кнопки"), []()
			{
				bind_button("Zeus Bot", u8"Zeus Bot", globals::binds::zeus_bot);
				bind_button("Slow Walk", "Slow Walk", globals::binds::slow_walk);
				bind_button("Fake Crouch", "Fake Crouch", globals::binds::fake_duck);
				bind_button("Third Person", "Third Person", globals::binds::thirdperson::key);
				bind_button("Lightning Shot", "Lightning Shot", globals::binds::lightning_shot);
				bind_button("Edge Jump", "Edge Jump", globals::binds::edge_jump);
				//bind_button("Block Bot", "Block Bot", globals::binds::block_bot);

				if (settings::misc::human_bhop)
				{
					separator(___("Humanised Bhop Settings", u8"Фейк лаги"));

					ImGui::SliderIntLeftAligned("Bhop Hit Chance", &settings::misc::bhop_hit_chance, 0, 100, "%.0f %%");
					tooltip("The chance of second hop, first hop is always at 100%");
					ImGui::SliderIntLeftAligned("Hops Limit", &settings::misc::hops_restricted_limit, 0, 12);
					tooltip("Will fuckup the bhop after certain amount of hops to look legit.");
					ImGui::SliderIntLeftAligned("Max Hops Limit", &settings::misc::max_hops_hit, 0, 12);
					tooltip("Will fuckup the bhop after certain amount of hops to look legit.");
				}

				/*if (!interfaces::local_player)
				{
					ImGui::Separator();

					char btn_label[256];
					sprintf_s(btn_label, "%s (%d)", ___("Invite nearby players", u8"Пригласить ближ. игроков"), lobby_inviter::max_count); //Not working anymore.

					if (ImGui::Button(btn_label, ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
						lobby_inviter::inviteAll();
				} */
			});
		}
	}
}