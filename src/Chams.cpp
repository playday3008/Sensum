#include "Chams.h"
#include "settings.h"
#include "features/features.h"
#include "hooks/hooks.h"
#include "Backtrack_new.h"
#include <fstream>

Chams::Chams() {
	std::ofstream("csgo\\materials\\simple_regular.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_ignorez.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	materialRegular = g::mat_system->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
	materialRegularIgnoreZ = g::mat_system->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
	materialFlatIgnoreZ = g::mat_system->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);
	materialFlat = g::mat_system->FindMaterial("simple_flat", TEXTURE_GROUP_MODEL);

	materialRegular->IncrementReferenceCount();
	materialRegularIgnoreZ->IncrementReferenceCount();
	materialFlatIgnoreZ->IncrementReferenceCount();
	materialFlat->IncrementReferenceCount();
}

//"$basetexture" "vgui/white_additive"

Chams::~Chams() {
	std::remove("csgo\\materials\\simple_regular.vmt");
	std::remove("csgo\\materials\\simple_ignorez.vmt");
	std::remove("csgo\\materials\\simple_flat.vmt");
	std::remove("csgo\\materials\\simple_flat_ignorez.vmt");
}

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

		/*
		if (utilities::is_behind_smoke(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_head)) && config_system.item.vis_chams_smoke_check)
					continue;
		*/

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
		g::render_view->SetColorModulation(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f);
		entity->GetClientRenderable()->DrawModel(0x1, 255);
		if (xqz || metallic_xqz || flat_xqz)
		{
			MaterialManager::Get().OverrideMaterial(false, flat, wireframe, glass, metallic);
			g::render_view->SetColorModulation(clr2.r() / 255.f, clr2.g() / 255.f, clr2.b() / 255.f);
			entity->GetClientRenderable()->DrawModel(0x1, 255);
		}
		g::mdl_render->ForcedMaterialOverride(nullptr);
	}

	g::mdl_render->ForcedMaterialOverride(nullptr);
}

void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba) {
	IMaterial* material = nullptr;

	if (flat) {
		if (ignoreZ)
			material = materialFlatIgnoreZ;
		else
			material = materialFlat;
	}
	else {
		if (ignoreZ)
			material = materialRegularIgnoreZ;
		else
			material = materialRegular;
	}


	if (glass) {
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else {
		material->AlphaModulate(
			rgba.a() / 255.0f);
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);

	g::mdl_render->ForcedMaterialOverride(material);
	material->IncrementReferenceCount();
}