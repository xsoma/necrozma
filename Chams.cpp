#include "Chams.h"
#include "Hooks.h"
#include "AnimationFix.h"
#include "Ragebot.h"
MatrixRecorded g_Records[64];
void matrix_set_origin(Vector pos, matrix& m)
{
	m[0][3] = pos.x;
	m[1][3] = pos.y;
	m[2][3] = pos.z;
}

Vector matrix_get_origin(const matrix& src)
{
	return { src[0][3], src[1][3], src[2][3] };
}

bool GetBacktrackMaxtrix(IBasePlayer* player, matrix* out)
{

	const auto last = g_Animfix->get_oldest_animation(player);

	if (!last.has_value())
		return false;

	memcpy(out, last.value()->bones, sizeof(matrix[128]));
	return true;

}

bool GetSmoothMatrix(IBasePlayer* player, matrix* out)
{
	auto Interpolate = [](const Vector from, const Vector to, const float percent) {
		return to * percent + from * ((1.f - percent));
	};

	auto first_invalid = csgo->all_data[1];
	auto last_valid = csgo->all_data[0];

	auto orig_delta = (last_valid.origin - first_invalid.origin).Length2D();
	const auto next = last_valid.origin;
	const auto curtime = interfaces.global_vars->curtime;

	const float correct = interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING)
		+ interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + Ragebot::Get().LerpTime() - (last_valid.sim_time - first_invalid.sim_time);

	float time_delta = first_invalid.sim_time - last_valid.sim_time;
	float add = 0.2f;
	float deadtime = last_valid.sim_time + correct + add;

	float delta = deadtime - curtime;

	float mul = 1.f / add;

	Vector lerp = Interpolate(first_invalid.origin, next, std::clamp(delta * mul, 0.f, 1.f));

	matrix ret[128];
	memcpy(ret, csgo->fakelag_matrix, sizeof(matrix) * 128);

	for (size_t i{}; i < 128; ++i)
	{
		auto matrix_delta = matrix_get_origin(csgo->fakelag_matrix[i]) - last_valid.origin;

		matrix_set_origin(matrix_delta + lerp, ret[i]);
	}

	memcpy(out, ret, sizeof(matrix) * 128);
	return true;
}


void CChams::Draw(void* ecx, void* results, const DrawModelInfo_t& info,
	matrix* bone_to_world, float* flex_weights, float* flex_delayed_weights, const Vector& model_origin, int flags)
{
	static auto DrawModel = g_pStudioRenderHook->GetOriginal<fnDrawModel>(29);
	const auto ModelName = info.pStudioHdr->name;
	static IMaterial* Regular = interfaces.material_system->FindMaterial(hs::debugambientcube::s().c_str(), nullptr);
	static IMaterial* Flat = interfaces.material_system->FindMaterial(hs::debugdrawflat::s().c_str(), nullptr);
	static IMaterial* Metallic = interfaces.material_system->FindMaterial(hs::regular_ref::s().c_str(), nullptr);
	static IMaterial* Glow = interfaces.material_system->FindMaterial(hs::glow_armsrace::s().c_str(), nullptr);
	static IMaterial* GlowFade = interfaces.material_system->FindMaterial(hs::regular_glow::s().c_str(), nullptr);

	// говнокод, мб потом переделаю 

	if (strstr(ModelName, "player/") != nullptr)
	{
		if (!info.pClientEntity)
			return DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

		auto Entity = info.pClientEntity->GetIClientUnknown()->GetBaseEntity();

		if (!Entity)
			return DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

		if (Entity && Entity->IsValid())
		{
			static IMaterial* OverlayMaterial = nullptr;
			static IMaterial* OverlayMaterialXqz = nullptr;

			static IMaterial* OverridedMaterial = nullptr;
			static IMaterial* OverridedMaterialXqz = nullptr;

			static IMaterial* OverridedMaterialBck = nullptr;
			static IMaterial* OverlayMaterialBck = nullptr;

			switch (vars.visuals.overlay)
			{
			case 1: OverlayMaterial = GlowFade; break;
			case 2: OverlayMaterial = Glow; break;
			}

			switch (vars.visuals.misc_chams[history].overlay)
			{
			case 1: OverlayMaterialBck = GlowFade; break;
			case 2: OverlayMaterialBck = Glow; break;
			}

			switch (vars.visuals.overlay_xqz)
			{
			case 1: OverlayMaterialXqz = GlowFade; break;
			case 2: OverlayMaterialXqz = Glow; break;
			}

			switch (vars.visuals.chamstype)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, true);

			switch (vars.visuals.misc_chams[history].material)
			{
			case 0: OverridedMaterialBck = Regular; break;
			case 1: OverridedMaterialBck = Flat; break;
			case 2: OverridedMaterialBck = Metallic; break;
			}

			matrix pMat[128];
			if (vars.visuals.misc_chams[history].enable && GetBacktrackMaxtrix(Entity, pMat)) {
				if (vars.visuals.misc_chams[history].material == 2)
				{
					auto phong = OverridedMaterialBck->FindVar(hs::phongexponent::s().c_str(), nullptr);
					phong->SetVecValue((float)vars.visuals.misc_chams[history].phong_exponent);
					auto phong2 = OverridedMaterialBck->FindVar(hs::phongboost::s().c_str(), nullptr);
					phong2->SetVecValue((float)vars.visuals.misc_chams[history].phong_boost);

					auto rim = OverridedMaterialBck->FindVar(hs::rimlightexponent::s().c_str(), nullptr);
					rim->SetVecValue((float)vars.visuals.misc_chams[history].rim);
					auto rim2 = OverridedMaterialBck->FindVar(hs::rimlightboost::s().c_str(), nullptr);
					rim2->SetVecValue((float)vars.visuals.misc_chams[history].rim);
					auto pearl = OverridedMaterial->FindVar(hs::pearlescent::s().c_str(), nullptr);
					pearl->SetVecValue(vars.visuals.misc_chams[history].pearlescent / 100.f);
					auto var = OverridedMaterialBck->FindVar(hs::envmaptint::s().c_str(), nullptr);
					var->SetVecValue(vars.visuals.misc_chams[history].metallic_clr[0] / 255.f,
						vars.visuals.misc_chams[history].metallic_clr[1] / 255.f, vars.visuals.misc_chams[history].metallic_clr[2] / 255.f);

					auto var2 = OverridedMaterialBck->FindVar(hs::phongtint::s().c_str(), nullptr);
					var2->SetVecValue(vars.visuals.misc_chams[history].metallic_clr2[0] / 255.f,
						vars.visuals.misc_chams[history].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[history].metallic_clr2[2] / 255.f);
				}

				OverridedMaterialBck->ColorModulate(
					vars.visuals.misc_chams[history].clr[0] / 255.f,
					vars.visuals.misc_chams[history].clr[1] / 255.f,
					vars.visuals.misc_chams[history].clr[2] / 255.f);
				OverridedMaterialBck->AlphaModulate(vars.visuals.misc_chams[history].clr[3] / 255.f);
				OverridedMaterialBck->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				float clr[3] = {
					(vars.visuals.misc_chams[history].clr[0] / 255.f) * vars.visuals.misc_chams[history].chams_brightness / 100.f,
					(vars.visuals.misc_chams[history].clr[1] / 255.f) * vars.visuals.misc_chams[history].chams_brightness / 100.f,
					(vars.visuals.misc_chams[history].clr[2] / 255.f) * vars.visuals.misc_chams[history].chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterialBck);
				DrawModel(ecx, results, info, pMat, flex_weights, flex_delayed_weights, model_origin, flags);

				if (vars.visuals.misc_chams[history].overlay > 0) {
					auto pVar = OverlayMaterialBck->FindVar(hs::envmaptint::s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.misc_chams[history].glow_clr[0] / 255.f,
						vars.visuals.misc_chams[history].glow_clr[1] / 255.f, vars.visuals.misc_chams[history].glow_clr[2] / 255.f);

					OverlayMaterialBck->AlphaModulate(vars.visuals.misc_chams[history].glow_clr[3] / 255.f);
					OverlayMaterialBck->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialBck);
					DrawModel(ecx, results, info, pMat, flex_weights, flex_delayed_weights, model_origin, flags);
				}
			}



			if (vars.visuals.chamstype == 2)
			{
				auto phong = OverridedMaterial->FindVar(hs::phongexponent::s().c_str(), nullptr);
				phong->SetVecValue((float)vars.visuals.phong_exponent);
				auto phong2 = OverridedMaterial->FindVar(hs::phongboost::s().c_str(), nullptr);
				phong2->SetVecValue((float)vars.visuals.phong_boost);

				auto rim = OverridedMaterial->FindVar(hs::rimlightexponent::s().c_str(), nullptr);
				rim->SetVecValue((float)vars.visuals.rim);
				auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost::s().c_str(), nullptr);
				rim2->SetVecValue((float)vars.visuals.rim);
				auto pearl = OverridedMaterial->FindVar(hs::pearlescent::s().c_str(), nullptr);
				pearl->SetVecValue((float)vars.visuals.pearlescent / 100.f);
				auto var = OverridedMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
				var->SetVecValue(vars.visuals.metallic_clr[0] / 255.f, vars.visuals.metallic_clr[1] / 255.f, vars.visuals.metallic_clr[2] / 255.f);

				auto var2 = OverridedMaterial->FindVar(hs::phongtint::s().c_str(), nullptr);
				var2->SetVecValue(vars.visuals.metallic_clr2[0] / 255.f, vars.visuals.metallic_clr2[1] / 255.f, vars.visuals.metallic_clr2[2] / 255.f);
			}

			if (vars.visuals.chamsxqz)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.chamscolor_xqz[0] / 255.f, vars.visuals.chamscolor_xqz[1] / 255.f, vars.visuals.chamscolor_xqz[2] / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.chamscolor_xqz[3] / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				float clr[3] = {
				(vars.visuals.chamscolor_xqz[0] / 255.f) * vars.visuals.chams_brightness / 100.f,
				(vars.visuals.chamscolor_xqz[1] / 255.f) * vars.visuals.chams_brightness / 100.f,
				(vars.visuals.chamscolor_xqz[2] / 255.f) * vars.visuals.chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);
				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

				if (vars.visuals.overlay_xqz > 0) {
					auto pVar = OverlayMaterialXqz->FindVar(hs::envmaptint::s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.glow_col_xqz[0] / 255.f, vars.visuals.glow_col_xqz[1] / 255.f, vars.visuals.glow_col_xqz[2] / 255.f);

					OverlayMaterialXqz->AlphaModulate(vars.visuals.glow_col_xqz[3] / 255.f);
					OverlayMaterialXqz->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialXqz);
					DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
					interfaces.render_view->SetColorModulation(backup_clr);
				}
			}

			if (vars.visuals.chams)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.chamscolor[0] / 255.f, vars.visuals.chamscolor[1] / 255.f, vars.visuals.chamscolor[2] / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.chamscolor[3] / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					(vars.visuals.chamscolor[0] / 255.f) * vars.visuals.chams_brightness / 100.f,
					(vars.visuals.chamscolor[1] / 255.f) * vars.visuals.chams_brightness / 100.f,
					(vars.visuals.chamscolor[2] / 255.f) * vars.visuals.chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);
				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

				if (vars.visuals.overlay > 0) {
					auto pVar = OverlayMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.glow_col[0] / 255.f, vars.visuals.glow_col[1] / 255.f, vars.visuals.glow_col[2] / 255.f);

					OverlayMaterial->AlphaModulate(vars.visuals.glow_col[3] / 255.f);
					OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
					DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
					interfaces.render_view->SetColorModulation(backup_clr);
				}
			}
		}
		else if (Entity == csgo->local && csgo->local->isAlive())
		{

			static IMaterial* OverridedMaterialLocal = nullptr;
			static IMaterial* OverlayMaterialLocal = nullptr;
			if (interfaces.input->m_fCameraInThirdPerson) {

				switch (vars.visuals.localchamstype)
				{
				case 0: OverridedMaterialLocal = Regular; break;
				case 1: OverridedMaterialLocal = Flat; break;
				case 2: OverridedMaterialLocal = Metallic; break;
				}

				switch (vars.visuals.local_chams.overlay)
				{
				case 1: OverlayMaterialLocal = GlowFade; break;
				case 2: OverlayMaterialLocal = Glow; break;
				}

				if (vars.visuals.blend_on_scope)
				{
					if (csgo->local->IsScoped())
						interfaces.render_view->SetBlend(vars.visuals.blend_value / 100.f);
				}
				/*for (auto& i : csgo->realmatrix)
				{
					i[0][3] += info.origin.x;
					i[1][3] += info.origin.y;
					i[2][3] += info.origin.z;
				}*/
				if (csgo->fakematrix != nullptr && !csgo->game_rules->IsFreezeTime())
				{

					for (auto& i : csgo->fakematrix)
					{
						i[0][3] += model_origin.x;
						i[1][3] += model_origin.y;
						i[2][3] += model_origin.z;
					}


					matrix mat1[128];
					if (GetSmoothMatrix(csgo->local, mat1))
					{
						static IMaterial* OverridedMaterial = nullptr;
						static IMaterial* OverlayMaterial = nullptr;

						switch (vars.visuals.misc_chams[desync].overlay)
						{
						case 1: OverlayMaterial = GlowFade; break;
						case 2: OverlayMaterial = Glow; break;
						}

						switch (vars.visuals.misc_chams[desync].material)
						{
						case 0: OverridedMaterial = Regular; break;
						case 1: OverridedMaterial = Flat; break;
						case 2: OverridedMaterial = Metallic; break;
						}

						if (vars.visuals.misc_chams[desync].material == 2)
						{
							auto phong = OverridedMaterial->FindVar(hs::phongexponent::s().c_str(), nullptr);
							phong->SetVecValue((float)vars.visuals.misc_chams[desync].phong_exponent);
							auto phong2 = OverridedMaterial->FindVar(hs::phongboost::s().c_str(), nullptr);
							phong2->SetVecValue((float)vars.visuals.misc_chams[desync].phong_boost);

							auto rim = OverridedMaterial->FindVar(hs::rimlightexponent::s().c_str(), nullptr);
							rim->SetVecValue((float)vars.visuals.misc_chams[desync].rim);
							auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost::s().c_str(), nullptr);
							rim2->SetVecValue((float)vars.visuals.misc_chams[desync].rim);
							auto pearl = OverridedMaterial->FindVar(hs::pearlescent::s().c_str(), nullptr);
							pearl->SetVecValue(vars.visuals.misc_chams[desync].pearlescent / 100.f);
							auto var = OverridedMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
							var->SetVecValue(vars.visuals.misc_chams[desync].metallic_clr[0] / 255.f,
								vars.visuals.misc_chams[desync].metallic_clr[1] / 255.f, vars.visuals.misc_chams[desync].metallic_clr[2] / 255.f);

							auto var2 = OverridedMaterial->FindVar(hs::phongtint::s().c_str(), nullptr);
							var2->SetVecValue(vars.visuals.misc_chams[desync].metallic_clr2[0] / 255.f,
								vars.visuals.misc_chams[desync].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[desync].metallic_clr2[2] / 255.f);
						}

						if (vars.visuals.misc_chams[desync].enable)
						{
							OverridedMaterial->ColorModulate(
								vars.visuals.misc_chams[desync].clr[0] / 255.f,
								vars.visuals.misc_chams[desync].clr[1] / 255.f, vars.visuals.misc_chams[desync].clr[2] / 255.f);

							OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[desync].clr[3] / 255.f);
							OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

							float clr[3] = {
								(vars.visuals.misc_chams[desync].clr[0] / 255.f) * vars.visuals.misc_chams[desync].chams_brightness / 100.f,
								(vars.visuals.misc_chams[desync].clr[1] / 255.f) * vars.visuals.misc_chams[desync].chams_brightness / 100.f,
								(vars.visuals.misc_chams[desync].clr[2] / 255.f) * vars.visuals.misc_chams[desync].chams_brightness / 100.f
							};

							float backup_clr[3];
							interfaces.render_view->GetColorModulation(backup_clr);
							interfaces.render_view->SetColorModulation(clr);
							interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
							DrawModel(ecx, results, info, !vars.visuals.interpolated_dsy ? csgo->fakematrix : mat1, flex_weights, flex_delayed_weights, model_origin, flags);
							interfaces.render_view->SetColorModulation(backup_clr);

							if (vars.visuals.misc_chams[desync].overlay > 0) {
								auto pVar = OverlayMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
								pVar->SetVecValue(vars.visuals.misc_chams[desync].glow_clr[0] / 255.f,
									vars.visuals.misc_chams[desync].glow_clr[1] / 255.f, vars.visuals.misc_chams[desync].glow_clr[2] / 255.f);

								OverlayMaterial->AlphaModulate(vars.visuals.misc_chams[desync].glow_clr[3] / 255.f);
								OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

								interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
								DrawModel(ecx, results, info, !vars.visuals.interpolated_dsy ? csgo->fakematrix : mat1, flex_weights, flex_delayed_weights, model_origin, flags);
								interfaces.models.model_render->ForcedMaterialOverride(nullptr);
								interfaces.render_view->SetColorModulation(backup_clr);
							}
							//
						}
					}

					for (auto& i : csgo->fakematrix)
					{
						i[0][3] -= model_origin.x;
						i[1][3] -= model_origin.y;
						i[2][3] -= model_origin.z;
					}

				}

				if (vars.visuals.localchams)
				{
					OverridedMaterialLocal->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, true);

					if (vars.visuals.localchamstype == 2)
					{
						auto phong = OverridedMaterialLocal->FindVar(hs::phongexponent::s().c_str(), nullptr);
						phong->SetVecValue((float)vars.visuals.local_chams.phong_exponent);
						auto phong2 = OverridedMaterialLocal->FindVar(hs::phongboost::s().c_str(), nullptr);
						phong2->SetVecValue((float)vars.visuals.local_chams.phong_boost);

						auto rim = OverridedMaterialLocal->FindVar(hs::rimlightexponent::s().c_str(), nullptr);
						rim->SetVecValue((float)vars.visuals.local_chams.rim);
						auto rim2 = OverridedMaterialLocal->FindVar(hs::rimlightboost::s().c_str(), nullptr);
						rim2->SetVecValue((float)vars.visuals.local_chams.rim);
						auto pearl = OverridedMaterialLocal->FindVar(hs::pearlescent::s().c_str(), nullptr);
						pearl->SetVecValue(vars.visuals.local_chams.pearlescent / 100.f);
						auto var = OverridedMaterialLocal->FindVar(hs::envmaptint::s().c_str(), nullptr);
						var->SetVecValue(vars.visuals.local_chams.metallic_clr[0] / 255.f,
							vars.visuals.local_chams.metallic_clr[1] / 255.f, vars.visuals.local_chams.metallic_clr[2] / 255.f);

						auto var2 = OverridedMaterialLocal->FindVar(hs::phongtint::s().c_str(), nullptr);
						var2->SetVecValue(vars.visuals.local_chams.metallic_clr2[0] / 255.f,
							vars.visuals.local_chams.metallic_clr2[1] / 255.f, vars.visuals.local_chams.metallic_clr2[2] / 255.f);
					}
					OverridedMaterialLocal->ColorModulate(
						vars.visuals.localchams_color[0] / 255.f, vars.visuals.localchams_color[1] / 255.f, vars.visuals.localchams_color[2] / 255.f);
					float clr[3] = {
						(vars.visuals.localchams_color[0] / 255.f) * vars.visuals.local_chams_brightness / 100.f,
						(vars.visuals.localchams_color[1] / 255.f) * vars.visuals.local_chams_brightness / 100.f,
						(vars.visuals.localchams_color[2] / 255.f) * vars.visuals.local_chams_brightness / 100.f
					};
					float backup_clr[3];
					interfaces.render_view->GetColorModulation(backup_clr);
					interfaces.render_view->SetColorModulation(clr);

					OverridedMaterialLocal->AlphaModulate(vars.visuals.localchams_color[3] / 255.f);
					OverridedMaterialLocal->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterialLocal);
					DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

					if (vars.visuals.local_chams.overlay > 0) {
						auto pVar = OverlayMaterialLocal->FindVar(hs::envmaptint::s().c_str(), nullptr);
						pVar->SetVecValue(vars.visuals.local_glow_color[0] / 255.f,
							vars.visuals.local_glow_color[1] / 255.f, vars.visuals.local_glow_color[2] / 255.f);

						OverlayMaterialLocal->AlphaModulate(vars.visuals.local_glow_color[3] / 255.f);
						OverlayMaterialLocal->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);

						interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialLocal);
						DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
						interfaces.render_view->SetColorModulation(backup_clr);
					}
				}
				else {
					interfaces.models.model_render->ForcedMaterialOverride(nullptr);
					DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
			}
		}
	}
	else if (strstr(ModelName, "arms") != nullptr)
	{
		if (csgo->local && csgo->local->isAlive() && !interfaces.input->m_fCameraInThirdPerson)
		{
			static IMaterial* OverridedMaterial = nullptr;
			static IMaterial* OverlayMaterial = nullptr;

			switch (vars.visuals.misc_chams[arms].overlay)
			{
			case 1: OverlayMaterial = GlowFade; break;
			case 2: OverlayMaterial = Glow; break;
			}

			switch (vars.visuals.misc_chams[arms].material)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			if (vars.visuals.misc_chams[arms].material == 2)
			{
				auto phong = OverridedMaterial->FindVar(hs::phongexponent::s().c_str(), nullptr);
				phong->SetVecValue((float)vars.visuals.misc_chams[arms].phong_exponent);
				auto phong2 = OverridedMaterial->FindVar(hs::phongboost::s().c_str(), nullptr);
				phong2->SetVecValue((float)vars.visuals.misc_chams[arms].phong_boost);

				auto rim = OverridedMaterial->FindVar(hs::rimlightexponent::s().c_str(), nullptr);
				rim->SetVecValue((float)vars.visuals.misc_chams[arms].rim);
				auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost::s().c_str(), nullptr);
				rim2->SetVecValue((float)vars.visuals.misc_chams[arms].rim);
				auto pearl = OverridedMaterial->FindVar(hs::pearlescent::s().c_str(), nullptr);
				pearl->SetVecValue(vars.visuals.misc_chams[arms].pearlescent / 100.f);
				auto var = OverridedMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
				var->SetVecValue(vars.visuals.misc_chams[arms].metallic_clr.get_red() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr.get_green() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr.get_blue() / 255.f);

				auto var2 = OverridedMaterial->FindVar(hs::phongtint::s().c_str(), nullptr);
				var2->SetVecValue(vars.visuals.misc_chams[arms].metallic_clr2.get_red() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr2.get_green() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr2.get_blue() / 255.f);
			}

			if (vars.visuals.misc_chams[arms].enable)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.misc_chams[arms].clr.get_red() / 255.f,
					vars.visuals.misc_chams[arms].clr.get_green() / 255.f,
					vars.visuals.misc_chams[arms].clr.get_blue() / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[arms].clr.get_alpha() / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					(vars.visuals.misc_chams[arms].clr.get_red() / 255.f) * vars.visuals.misc_chams[arms].chams_brightness / 100.f,
					(vars.visuals.misc_chams[arms].clr.get_green() / 255.f) * vars.visuals.misc_chams[arms].chams_brightness / 100.f,
					(vars.visuals.misc_chams[arms].clr.get_blue() / 255.f) * vars.visuals.misc_chams[arms].chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

				if (vars.visuals.misc_chams[arms].overlay > 0) {
					auto pVar = OverlayMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.misc_chams[arms].glow_clr[0] / 255.f,
						vars.visuals.misc_chams[arms].glow_clr[1] / 255.f, vars.visuals.misc_chams[arms].glow_clr[2] / 255.f);

					OverlayMaterial->AlphaModulate(vars.visuals.misc_chams[arms].glow_clr[3] / 255.f);
					OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
					DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
			}
		}
	}
	else if (strstr(ModelName, "v_") != nullptr)
	{
		if (csgo->local && csgo->local->isAlive() && !interfaces.input->m_fCameraInThirdPerson)
		{
			static IMaterial* OverridedMaterial = nullptr;
			static IMaterial* OverlayMaterial = nullptr;

			switch (vars.visuals.misc_chams[weapon].overlay)
			{
			case 1: OverlayMaterial = GlowFade; break;
			case 2: OverlayMaterial = Glow; break;
			}

			switch (vars.visuals.misc_chams[weapon].material)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			if (vars.visuals.misc_chams[weapon].material == 2)
			{
				auto phong = OverridedMaterial->FindVar(hs::phongexponent::s().c_str(), nullptr);
				phong->SetVecValue((float)vars.visuals.misc_chams[weapon].phong_exponent);
				auto phong2 = OverridedMaterial->FindVar(hs::phongboost::s().c_str(), nullptr);
				phong2->SetVecValue((float)vars.visuals.misc_chams[weapon].phong_boost);

				auto rim = OverridedMaterial->FindVar(hs::rimlightexponent::s().c_str(), nullptr);
				rim->SetVecValue((float)vars.visuals.misc_chams[weapon].rim);
				auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost::s().c_str(), nullptr);
				rim2->SetVecValue((float)vars.visuals.misc_chams[weapon].rim);
				auto pearl = OverridedMaterial->FindVar(hs::pearlescent::s().c_str(), nullptr);
				pearl->SetVecValue(vars.visuals.misc_chams[weapon].pearlescent / 100.f);
				auto var = OverridedMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
				var->SetVecValue(vars.visuals.misc_chams[weapon].metallic_clr[0] / 255.f,
					vars.visuals.misc_chams[weapon].metallic_clr[1] / 255.f, vars.visuals.misc_chams[weapon].metallic_clr[2] / 255.f);

				auto var2 = OverridedMaterial->FindVar(hs::phongtint::s().c_str(), nullptr);
				var2->SetVecValue(vars.visuals.misc_chams[weapon].metallic_clr2[0] / 255.f,
					vars.visuals.misc_chams[weapon].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[weapon].metallic_clr2[2] / 255.f);
			}

			if (vars.visuals.misc_chams[weapon].enable)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.misc_chams[weapon].clr[0] / 255.f, vars.visuals.misc_chams[weapon].clr[1] / 255.f, vars.visuals.misc_chams[weapon].clr[2] / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[weapon].clr[3] / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					(vars.visuals.misc_chams[weapon].clr[0] / 255.f) * vars.visuals.misc_chams[weapon].chams_brightness / 100.f,
					(vars.visuals.misc_chams[weapon].clr[1] / 255.f) * vars.visuals.misc_chams[weapon].chams_brightness / 100.f,
					(vars.visuals.misc_chams[weapon].clr[2] / 255.f) * vars.visuals.misc_chams[weapon].chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);

				if (vars.visuals.misc_chams[weapon].overlay > 0) {
					auto pVar = OverlayMaterial->FindVar(hs::envmaptint::s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.misc_chams[weapon].glow_clr[0] / 255.f,
						vars.visuals.misc_chams[weapon].glow_clr[1] / 255.f, vars.visuals.misc_chams[weapon].glow_clr[2] / 255.f);

					OverlayMaterial->AlphaModulate(vars.visuals.misc_chams[weapon].glow_clr[3] / 255.f);
					OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
					DrawModel(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
				}
				interfaces.render_view->SetColorModulation(backup_clr);
			}
		}
	}
}