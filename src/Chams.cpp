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

/*void Chams::OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld,
	float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags) {
	static auto fnDME = hooks::mdlrender::hook.get_original<hooks::mdlrender::draw_model_execute::DrawModelExecute>(hooks::mdlrender::draw_model_execute::index);

	if (!pInfo->m_pClientEntity || !g::local_player)
		return;

	const auto mdl = pInfo->m_pClientEntity->GetModel();

	float hue = 266.f; //276

	if (settings::chams::bttype == 1 || settings::chams::bttype == 2 && strstr(mdl->szName, "models/player") != nullptr) {

		auto ent = (c_base_player*)(pInfo->m_pClientEntity->GetIClientUnknown()->GetBaseEntity());

		if (ent && ent->IsPlayer() && ent->IsAlive()) {
			const auto enemy = ent->m_iTeamNum() != g::local_player->m_iTeamNum();
			if (!enemy)
				return;

			if (settings::chams::bttype && g_Backtrack.data.count(ent->EntIndex()) > 0) {
				auto& data = g_Backtrack.data.at(ent->EntIndex());

				if (data.size() > 0) {
					if (settings::chams::bttype == 2) {
						for (auto record : data)
						{
							MaterialManager::Get().OverrideMaterial(false, settings::chams::btflat, false, false, false, Color(settings::chams::btColorChams));
							fnDME(g::g_studiorender, pResults, pInfo, record.boneMatrix, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}
					}
					else if (settings::chams::bttype == 1) {
						auto& back = data.back();
						MaterialManager::Get().OverrideMaterial(false, settings::chams::btflat, false, false, false, Color(settings::chams::btColorChams));
						fnDME(g::g_studiorender, pResults, pInfo, back.boneMatrix, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
					}
				}
			}
		}
	}
} */

/*void Chams::DMEChams(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld,
	float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags) {
	static auto fnDME = hooks::mdlrender::hook.get_original<hooks::mdlrender::draw_model_execute::DrawModelExecute>(hooks::mdlrender::draw_model_execute::index);

		if (!pInfo->m_pClientEntity || !g::local_player)
		return;

		const auto mdl = pInfo->m_pClientEntity->GetModel();

		IMaterial* mat = g::mat_system->FindMaterial("sensum_reflective", TEXTURE_GROUP_MODEL);

		if (!mat)
			return;

		if (settings::chams::desync2 && strstr(mdl->szName, "models/player") != nullptr)
		{
			auto ent = (c_base_player*)(pInfo->m_pClientEntity->GetIClientUnknown()->GetBaseEntity());

			if (ent && ent == g::local_player && !ent->IsDormant() && ent->IsAlive())
			{
				g::mdl_render->ForcedMaterialOverride(mat);

				fnDME(g::g_studiorender, pResults, pInfo, matrix, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
			}
		}

} */