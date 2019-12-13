#include "Chams.h"
#include "settings.h"
#include "features/features.h"
#include "hooks/hooks.h"
#include "Backtrack_new.h"
#include <fstream>
#include "esp.hpp"

void Chams::OnSceneEnd()
{
	if (!g::engine_client->IsInGame() || !g::engine_client->IsConnected() || !g::local_player)
		return;

	ChamsModes LocalChamsMode = (ChamsModes)settings::chams::localmodenew;
	ChamsModes TeamChamsMode = (ChamsModes)settings::chams::teammodenew;
	ChamsModes EnemyChamsMode = (ChamsModes)settings::chams::enemymodenew;

	bool LocalChams = settings::chams::localnew;
	bool TeamChams = settings::chams::teamnew;
	bool EnemyChams = settings::chams::enemynew;

	Color LocalColor = settings::chams::LocalColor_vis;
	Color TeamColor = settings::chams::TeamColor_vis;
	Color EnemyColor = settings::chams::EnemyColor_vis;

	Color LocalColorXqz = settings::chams::LocalColor_XQZ;
	Color TeamColorXqz = settings::chams::TeamColor_XQZ;
	Color EnemyColorXqz = settings::chams::EnemyColor_XQZ;

	for (int i = 1; i < g::engine_client->GetMaxClients(); i++)
	{
		auto entity = static_cast<c_base_player*>(g::entity_list->GetClientEntity(i));
		if (!entity || !entity->IsPlayer() || entity->IsDormant() || !entity->IsAlive())
			continue;

		if (utils::is_line_goes_through_smoke(interfaces::local_player->GetEyePos(), entity->get_hitbox_position(entity, HITBOX_HEAD))) //GetRenderOrigin()
			continue;

		bool IsLocal = entity == g::local_player;
		bool IsTeam = !entity->IsEnemy();

		bool normal = false;
		bool flat = false;
		bool wireframe = false;
		bool glass = false;
		bool metallic = false;
		bool xqz = false;
		bool metallic_xqz = false;
		bool flat_xqz = false;
		bool glow = false;
		bool glow_xqz = false;

		ChamsModes mode = IsLocal ? LocalChamsMode : (IsTeam ? TeamChamsMode : EnemyChamsMode);

		if (IsLocal && !LocalChams)
		{
			continue;
		}
		if ((IsTeam && !IsLocal) && !TeamChams)
		{
			continue;
		}
		if (!IsTeam && !EnemyChams)
		{
			continue;
		}

		Color clr = IsLocal ? LocalColor : (IsTeam ? TeamColor : EnemyColor);
		Color clr2 = IsLocal ? LocalColorXqz : (IsTeam ? TeamColorXqz : EnemyColorXqz);

		switch (mode)
		{
		case ChamsModes::NORMAL:
			normal = true;
			break;
		case ChamsModes::FLAT:
			flat = true;
			break;
		case ChamsModes::WIREFRAME:
			wireframe = true;
			break;
		case ChamsModes::GLASS:
			glass = true;
			break;
		case ChamsModes::METALLIC:
			metallic = true;
			break;
		case ChamsModes::XQZ:
			xqz = true;
			break;
		case ChamsModes::METALLIC_XQZ:
			metallic = true;
			metallic_xqz = true;
			break;
		case ChamsModes::FLAT_XQZ:
			flat = true;
			flat_xqz = true;
			break;
		}

		int health = entity->m_iHealth();
		
		if (settings::chams::health_chams)
		{
			if (health == 100 || health >= 81)
				clr2 = Color(5, 255, 0, 255);
			else if (health == 80 || health >= 61)
				clr2 = Color::Yellow;
			else if (health == 60 || health >= 21)
				clr2 = Color(255, 127, 0, 255); //Orange
			else if (health <= 20)
				clr2 = Color::Red;
		}

		MaterialManager::Get().OverrideMaterial(xqz || metallic_xqz || flat_xqz, flat, wireframe, glass, metallic);
		g::render_view->SetColorModulation(clr2.r() / 255.f, clr2.g() / 255.f, clr2.b() / 255.f);
		entity->GetClientRenderable()->DrawModel(0x1, 255);
		if (xqz || metallic_xqz || flat_xqz || glow_xqz)
		{
			MaterialManager::Get().OverrideMaterial(false, flat, wireframe, glass, metallic);
			g::render_view->SetColorModulation(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f);
			entity->GetClientRenderable()->DrawModel(0x1, 255);
		}
		g::mdl_render->ForcedMaterialOverride(nullptr);
	}
	g::mdl_render->ForcedMaterialOverride(nullptr);
}