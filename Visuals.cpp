#include "Visuals.h"
#include "Autowall.h"
#include "Resolver.h"
#include "FakelagGraph.h"
#include "AntiAims.h"
#include "Ragebot.h"
#include "render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx9.h"
void CVisuals::ModelChanger()
{
	constexpr auto getModel = [](int team) constexpr noexcept -> const char* {
		constexpr std::array models{
("models/player/custom_player/legacy/ctm_fbi_variantb.mdl"), //Agent Ava
("models/player/custom_player/legacy/ctm_fbi_variantf.mdl"), //Operator
("models/player/custom_player/legacy/ctm_fbi_variantg.mdl"), //Markus Delrow
("models/player/custom_player/legacy/ctm_fbi_varianth.mdl"), //Michael Syfers
("models/player/custom_player/legacy/ctm_st6_variante.mdl"), //Seal Team 6 Soldier
("models/player/custom_player/legacy/ctm_st6_variantg.mdl"), //Buckshot
("models/player/custom_player/legacy/ctm_st6_varianti.mdl"), //Ricksaw
("models/player/custom_player/legacy/ctm_st6_variantk.mdl"), //3rd Commando Company
("models/player/custom_player/legacy/ctm_st6_variantm.mdl"), //McCoy
("models/player/custom_player/legacy/ctm_sas_variantf.mdl"), //B Squadron Officer
("models/player/custom_player/legacy/tm_leet_variantf.mdl"), //Mr Muhlik
("models/player/custom_player/legacy/tm_leet_varianti.mdl"), //Prof. Shahmat
("models/player/custom_player/legacy/tm_leet_varianth.mdl"), //Osiris
("models/player/custom_player/legacy/tm_leet_variantg.mdl"), //Ground Rebel
("models/player/custom_player/legacy/tm_balkan_variantf.mdl"), //Dragomir
("models/player/custom_player/legacy/tm_balkan_variantg.mdl"), //Rezan The Ready
("models/player/custom_player/legacy/tm_balkan_varianth.mdl"), //Doctor Romanov
("models/player/custom_player/legacy/tm_balkan_varianti.mdl"), //Maximus
("models/player/custom_player/legacy/tm_balkan_variantj.mdl"), //Blackwolf
("models/player/custom_player/legacy/tm_phoenix_variantf.mdl"), //Enforcer
("models/player/custom_player/legacy/tm_phoenix_variantg.mdl"), //Slingshot
("models/player/custom_player/legacy/tm_phoenix_varianth.mdl"), //Soldier Phoenix
("models/player/custom_player/legacy/ctm_heavy.mdl"), //Heavy Soldier
		};

		switch (team) {
		case 2: return static_cast<std::size_t>(vars.visuals.modelt - 1) < models.size() ? models[vars.visuals.modelt - 1] : nullptr;
		case 3: return static_cast<std::size_t>(vars.visuals.modelct - 1) < models.size() ? models[vars.visuals.modelct - 1] : nullptr;
		default: return nullptr;
		}
	};
	if (vars.visuals.localmodel)
	{
		static int originalIdx = 0;

		if (!csgo->local) {
			originalIdx = 0;
			return;
		}
		if (const auto model = getModel(csgo->local->GetTeam()))
		{
			originalIdx = csgo->local->m_nModelIndex();

			const auto idx = interfaces.models.model_info->GetModelIndex(model);

			csgo->local->setModelIndex(idx);

			if (const auto ragdoll = interfaces.ent_list->GetClientEntityFromHandle(csgo->local->ragdoll()))
				ragdoll->setModelIndex(idx);
		}
		//csgo->local->setModelIndex(interfaces.models.model_info->GetModelIndex(models[vars.visuals.modelt - 1]));
	}
}
bool GetBox(IBasePlayer* entity, int& x, int& y, int& w, int& h, Vector origin)
{
	if (entity->GetClientClass() && entity->GetClientClass()->m_ClassID == ClassId->CCSPlayer) {
		auto min = entity->GetCollideable()->OBBMins();
		auto max = entity->GetCollideable()->OBBMaxs();

		Vector dir, vF, vR, vU;

		interfaces.engine->GetViewAngles(dir);
		dir.x = 0;
		dir.z = 0;
		//dir.Normalize();
		//printf("%.1f\n", dir.y);
		Math::AngleVectors(dir, &vF, &vR, &vU);

		auto zh = vU * max.z + vF * max.y + vR * min.x; // = Front left front
		auto e = vU * max.z + vF * max.y + vR * max.x; //  = Front right front
		auto d = vU * max.z + vF * min.y + vR * min.x; //  = Front left back
		auto c = vU * max.z + vF * min.y + vR * max.x; //  = Front right back

		auto g = vU * min.z + vF * max.y + vR * min.x; //  = Bottom left front
		auto f = vU * min.z + vF * max.y + vR * max.x; //  = Bottom right front
		auto a = vU * min.z + vF * min.y + vR * min.x; //  = Bottom left back
		auto b = vU * min.z + vF * min.y + vR * max.x; //  = Bottom right back*-

		Vector pointList[] = {
			a,
			b,
			c,
			d,
			e,
			f,
			g,
			zh,
		};

		Vector transformed[ARRAYSIZE(pointList)];

		for (int i = 0; i < ARRAYSIZE(pointList); i++)
		{
			pointList[i] += origin;

			if (!Math::WorldToScreen2(pointList[i], transformed[i]))
				return false;
		}

		float left = FLT_MAX;
		float top = -FLT_MAX;
		float right = -FLT_MAX;
		float bottom = FLT_MAX;
		for (int i = 0; i < ARRAYSIZE(pointList); i++) {
			if (left > transformed[i].x)
				left = transformed[i].x;
			if (top < transformed[i].y)
				top = transformed[i].y;
			if (right < transformed[i].x)
				right = transformed[i].x;
			if (bottom > transformed[i].y)
				bottom = transformed[i].y;
		}

		x = left;
		y = bottom;
		w = right - left;
		h = top - bottom;

		return true;
	}
	else
	{
		Vector vOrigin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
		//float left, top, right, bottom;

		auto collideable = entity->GetCollideable();

		if (!collideable)
			return false;

		min = collideable->OBBMins();
		max = collideable->OBBMaxs();

		auto& trans = entity->GetrgflCoordinateFrame();

		Vector points[] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			Math::VectorTransform(points[i], trans, pointsTransformed[i]);
		}

		Vector pos = entity->GetRenderOrigin();

		if (!Math::WorldToScreen(pointsTransformed[3], flb) || !Math::WorldToScreen(pointsTransformed[5], brt)
			|| !Math::WorldToScreen(pointsTransformed[0], blb) || !Math::WorldToScreen(pointsTransformed[4], frt)
			|| !Math::WorldToScreen(pointsTransformed[2], frb) || !Math::WorldToScreen(pointsTransformed[1], brb)
			|| !Math::WorldToScreen(pointsTransformed[6], blt) || !Math::WorldToScreen(pointsTransformed[7], flt))
			return false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		//+1 for each cuz of borders at the original box
		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for (int i = 1; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (bottom < arr[i].y)
				bottom = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (top > arr[i].y)
				top = arr[i].y;
		}

		x = (int)left;
		y = (int)top;
		w = (int)(right - left);
		h = (int)(bottom - top);

		return true;
	}
	return false;
}

void CVisuals::DrawAngleLine(Vector origin, float angle, color_t color)
{
	Vector src, dst, sc1, sc2, forward;

	src = origin;
	Math::AngleVectors(Vector(0, angle, 0), &forward);
	if (Math::WorldToScreen(src, sc1) && Math::WorldToScreen(src + (forward * 40), sc2))
	{
		Drawing::DrawLine(sc1.x, sc1.y, sc2.x, sc2.y, color);
		//g_Render->DrawLine(sc1.x, sc1.y, sc2.x, sc2.y, color, 1.f);
	}
}
#include <time.h>
void CVisuals::DrawLocalShit(IDirect3DDevice9* pDevice)
{
	if (csgo->local && csgo->local->isAlive() && csgo->weapon && interfaces.engine->IsConnected() && interfaces.engine->IsInGame())
	{
		int dx = csgo->w, dy = csgo->h;

		if (vars.visuals.remove & 8)
		{
			if (csgo->weapon->isSniper() && csgo->local->IsScoped())
			{
				g_Render->DrawLine(dx / 2, 0, dx / 2, dy, color_t(0, 0, 0, 150), vars.visuals.scope_thickness);
				g_Render->DrawLine(0, dy / 2, dx, dy / 2, color_t(0, 0, 0, 150), vars.visuals.scope_thickness);
			}
		}

		static auto percent_col = [](int per) -> color_t {
			int red = per < 50 ? 255 : floorf(255 - (per * 2 - 100) * 255.f / 100.f);
			int green = per > 50 ? 255 : floorf((per * 2) * 255.f / 100.f);

			return color_t(red, green, 0);
		};

		int add = 0;

		auto anims = csgo->local->GetPlayerAnimState();

		if (vars.antiaim.enable && !csgo->ForceOffAA)
		{
			if (vars.visuals.indicators & 1 && vars.antiaim.enable)
			{
				float delta = csgo->delta;
				g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255 - (delta * 2.29824561404), delta * 3.42105263158, delta * 0.22807017543), render::outline, fonts::lby_indicator, "FAKE");

				add += 24;
			}
			if (vars.ragebot.force_body->active)
			{
				g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255, 255, 255), render::outline, fonts::lby_indicator, "BAIM");
				add += 24;
			}
			if (vars.visuals.indicators & 2 && vars.antiaim.fakelag > 0)
			{
				static color_t clr;
				if (csgo->canDrawLC && !vars.ragebot.double_tap->active)
				{
					clr = csgo->canBreakLC ? color_t(129, 190, 10) : color_t(255, 0, 0);
					g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, clr,
						render::outline, fonts::lby_indicator, "LC"
					);
					add += 24;
				}
			}
			if (vars.visuals.indicators & 4 && vars.antiaim.fakeduck->active)
			{
				g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255, 255, 255),
					render::outline, fonts::lby_indicator, "DUCK"
				);
				add += 24;
			}
		}
		if (vars.ragebot.enable)
		{
			if (vars.visuals.indicators & 8 && vars.ragebot.override_dmg->active) {
				g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255, 255, 255),
					render::outline, fonts::lby_indicator, "DMG"
				);
				add += 24;
			}
			if (vars.visuals.indicators & 16)
			{
				if (CanDT())
				{
					if (csgo->dt_charged && !CMAntiAim::Get().did_shot)
					{
						g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255, 255, 255),
							render::outline, fonts::lby_indicator, "DT"
						);
					}
					else
					{
						g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255, 0, 0),
							render::outline, fonts::lby_indicator, "DT"
						);
					}
					if (!csgo->dt_charged)
					{
						g_Render->FilledRect(dx - (dx - 11), dy / 1.5 + add + 20, 20, 2, color_t(0, 0, 0, 200));
						g_Render->FilledRect(dx - (dx - 11), dy / 1.5 + add + 20, csgo->skipped_ticks * 1.17, 2, color_t(0, 255, 0, 255));
					}
					add += 24;
				}
			}

			if (csgo->should_stop_slide)
			{
				g_Render->DrawString(dx - (dx - 10), dy / 1.5 + add, color_t(255, 255, 255),
					render::outline, fonts::lby_indicator, "STOP"
				);
			}
		}

		if (vars.visuals.watermark)
		{
			int width, height;
			interfaces.engine->GetScreenSize(width, height); // owowowo
			auto net_channel = interfaces.engine->GetNetChannelInfo();
			std::string incoming = csgo->local ? std::to_string((int)(net_channel->GetLatency(FLOW_INCOMING) * 1000)) : "0";

			if (vars.visuals.water_type == 0)
			{
				std::stringstream ss;
				std::stringstream rr;

				//getting the user
				char path[MAX_PATH], * name = NULL;
				GetEnvironmentVariable((LPCTSTR)"USERPROFILE", (LPSTR)path, (DWORD)MAX_PATH);
				name = &path[std::strlen(path) - 1];
				for (; *name != '\\'; --name);
				++name;

				//fps
				static int fps, old_tick_count;
				if ((interfaces.global_vars->tickcount - old_tick_count) > 50) {

					fps = static_cast<int>(1.f / interfaces.global_vars->frametime);

					old_tick_count = interfaces.global_vars->tickcount;

				}
				//rectangles and all that shit
				g_Render->FilledRect(dx - 290, 12, 260, 30, color_t(40, 40, 40, 225));
				g_Render->FilledRect(dx - 285, 17, 250, 20, color_t(10, 10, 10, 225));
				g_Render->Rect(dx - 285, 17, 250, 20, color_t(200, 200, 200, 50));
				g_Render->Rect(dx - 290, 12, 260, 30, color_t(200, 200, 200, 50));
				g_Render->filled_rect_gradient(dx - 284, 17, 142, 1, color_t(65, 165, 194), color_t(180, 80, 165), color_t(160, 50, 146), color_t(23, 134, 166));
				g_Render->filled_rect_gradient(dx - 162, 17, 126, 1, color_t(180, 80, 165), color_t(195, 207, 78), color_t(153, 166, 21), color_t(160, 50, 146));


				g_Render->DrawString(dx - 279, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, "game");
				g_Render->DrawString(dx - 250, 20.5, color_t(124, 193, 21), 0, fonts::menu_desc, " sense");
				g_Render->DrawString(dx - 205, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, " |");
				g_Render->DrawString(dx - 190, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, name);
				g_Render->DrawString(dx - 160, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, "   |");
				ss << fps;
				ImVec2 TextSize = ImGui::CalcTextSize(ss.str().c_str());
				g_Render->DrawString(width - 140, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, ss.str().c_str());
				if (fps < 100)
					g_Render->DrawString(width - 125, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, " fps");
				else
					g_Render->DrawString(width - 120, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, " fps");
				g_Render->DrawString(width - 105, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, "    |");
				rr << incoming;
				ImVec2 Uau = ImGui::CalcTextSize(rr.str().c_str());
				g_Render->DrawString(width - 80, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, rr.str().c_str());
				g_Render->DrawString(width - 65, 20.5, color_t(255, 255, 255), 0, fonts::menu_desc, " ms");
			}
			if (vars.visuals.water_type == 1)
			{
				std::stringstream rtt;
				std::stringstream rate;
				std::stringstream data;

				struct tm* tim;
				time_t tt = time(NULL);
				tim = localtime(&tt);
				int Hour = tim->tm_hour;
				int Min = tim->tm_min;
				int Sec = tim->tm_sec;

				rtt << incoming << "ms |";
				rate << "rate: " << 1.f / interfaces.global_vars->interval_per_tick << " |";
				data << Hour << ":" << Min << ":" << Sec;

				g_Render->FilledRect(dx - 290, 10, 280, 20, color_t(177, 79, 102, 180));
				auto ts = ImGui::CalcTextSize("supremacy | rtt: ");
				g_Render->DrawString(width - 285, 20, color_t(200, 200, 200, 200), render::centered_y, fonts::menu_desc, "supremacy | rtt: ");
				g_Render->DrawString(width - (285 - ts.x), 20, color_t(200, 200, 200, 200), render::centered_y, fonts::menu_desc, rtt.str().c_str());
				g_Render->DrawString(width - (280 - ts.x - 40), 20, color_t(200, 200, 200, 200), render::centered_y, fonts::menu_desc, rate.str().c_str());
				g_Render->DrawString(width - (280 - ts.x - 100), 20, color_t(200, 200, 200, 200), render::centered_y, fonts::menu_desc, data.str().c_str());
			}
		}
	}
}

std::string str_toupper(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return toupper(c); }
	);
	return s;
}

void DrawFovArrows(IBasePlayer* entity, float alpha, Vector origin)
{
	if (!vars.visuals.out_of_fov)
		return;

	if (!csgo->local->isAlive())
		return;

	auto idx = entity->GetIndex() - 1;
	Vector poopvec;

	Vector vEnemyOrigin = origin;
	Vector vLocalOrigin = csgo->origin;

	Vector screenPos;

	auto isOnScreen = [](Vector origin, Vector& screen) -> bool
	{
		if (!Math::WorldToScreen(origin, screen))
			return false;

		return (csgo->w > screen.x && screen.x > 0) && (csgo->h > screen.y && screen.y > 0);
	};

	if (!entity->IsDormant()) {
		if (isOnScreen(entity->GetBonePos(entity->GetBoneCache().Base(), 2), screenPos))
			return;
		else
			if (isOnScreen(vEnemyOrigin, screenPos))
				return;

	}
	Vector dir;
	interfaces.engine->GetViewAngles(dir);

	float view_angle = dir.y;

	if (view_angle < 0.f)
	{
		view_angle += 360.f;
	}
	view_angle = DEG2RAD(view_angle);

	auto entity_angle = Math::CalculateAngle(vLocalOrigin, vEnemyOrigin);
	entity_angle.Normalized();

	if (entity_angle.y < 0.f) {
		entity_angle.y += 360.f;
	}
	entity_angle.y = DEG2RAD(entity_angle.y);
	entity_angle.y -= view_angle;

	float dist = vars.visuals.out_of_fov_distance * 10;

	auto position = Vector2D(csgo->w / 2, csgo->h / 2);
	position.x -= dist;

	auto clr = entity->IsDormant() ? color_t(50, 50, 50, alpha - 50) : color_t(
		vars.visuals.out_of_fov_color[0],
		vars.visuals.out_of_fov_color[1],
		vars.visuals.out_of_fov_color[2],
		vars.visuals.out_of_fov_color[3] * (alpha / 255.f));

	Drawing::rotate_point(position, Vector2D(csgo->w / 2, csgo->h / 2), false, entity_angle.y);

	auto size = vars.visuals.out_of_fov_size / 2;


	Drawing::filled_tilted_triangle(position, Vector2D(size - 1, size), position, true, -entity_angle.y,
		(entity->IsDormant() && alpha > 255.f ? color_t(130, 130, 130, alpha) : clr));
}
bool IsProjectileS(IBasePlayer* m_entity)
{
	if (m_entity->GetClientClass()->m_ClassID == ClassId->CSmokeGrenadeProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CSensorGrenadeProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CMolotovProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CDecoyProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CBaseCSGrenadeProjectile
		)
		return true;
}
void Nades(IBasePlayer* Entity)
{
	Vector origin_screen;

	color_t color = color_t(255, 255, 255, 255);
	if (vars.visuals.glow)
	{
		BOX box;
		if (!GetBox(Entity, box.x, box.y, box.w, box.h, Entity->GetOrigin()))
			return;

		if (Entity->GetClientClass()->m_ClassID == ClassId->CSmokeGrenadeProjectile)
		{
			Drawing::DrawString(fonts::esp_info, origin_screen.x, origin_screen.y - 10, color, render::centered_x | render::centered_y, "smoke");
		}
		if (Entity->GetClientClass()->m_ClassID == ClassId->CMolotovProjectile)
		{
			Drawing::DrawString(fonts::esp_info, origin_screen.x, origin_screen.y - 10, color, render::centered_x | render::centered_y, "molotov");
		}
		if (Entity->GetClientClass()->m_ClassID == ClassId->CDecoyProjectile)
		{
			Drawing::DrawString(fonts::esp_info, origin_screen.x, origin_screen.y - 10, color, render::centered_x | render::centered_y, "decoy");
		}
		if (Entity->GetClientClass()->m_ClassID == ClassId->CBaseCSGrenadeProjectile)
		{
			Drawing::DrawString(fonts::esp_info, origin_screen.x, origin_screen.y - 10, color, render::centered_x | render::centered_y, "he grenade");
		}
	}
}
void Bombtimer(IBasePlayer* Entity)
{
}
void CVisuals::Draw()
{
	if (!interfaces.engine->IsConnected() && !interfaces.engine->IsInGame())
		return;

	RecordInfo();
	if (vars.visuals.enable)
	{
		for (auto entity : players)
		{
			Nades(entity);


			if (entity == nullptr
				|| !entity->IsPlayer()
				|| entity == csgo->local
				|| entity->GetTeam() == csgo->local->GetTeam())
				continue;

			bool dormant = entity->IsDormant();
			if (vars.visuals.dormant)
			{
				if (!entity->isAlive())
				{
					csgo->EntityAlpha[entity->GetIndex()] = 0;
					continue;
				}
				strored_origin[entity->GetIndex()] = entity->GetRenderOrigin();
				if (dormant)
				{
					if (csgo->EntityAlpha[entity->GetIndex()] > 140)
						csgo->EntityAlpha[entity->GetIndex()] -= 0.5f;
					else if ((csgo->LastSeenTime[entity->GetIndex()] + 4.f) <= csgo->get_absolute_time() && csgo->EntityAlpha[entity->GetIndex()] > 0)
						csgo->EntityAlpha[entity->GetIndex()] -= 0.5f;

					if (csgo->EntityAlpha[entity->GetIndex()] <= 0.f)
						continue;
				}
				else
				{
					csgo->EntityAlpha[entity->GetIndex()] = 255;
					csgo->LastSeenTime[entity->GetIndex()] = csgo->get_absolute_time();
				}

				csgo->EntityAlpha[entity->GetIndex()] = std::clamp(csgo->EntityAlpha[entity->GetIndex()], 0.f, 255.f);
			}
			else
			{
				if (dormant || !entity->isAlive())
					continue;

				strored_origin[entity->GetIndex()] = entity->GetRenderOrigin();
			}

			DrawFovArrows(entity, csgo->EntityAlpha[entity->GetIndex()], strored_origin[entity->GetIndex()]);

			int alpha = vars.visuals.dormant ? csgo->EntityAlpha[entity->GetIndex()] : 255;

			auto weapon = entity->GetWeapon();
			if (!weapon)
				continue;

			BOX box;
			if (!GetBox(entity, box.x, box.y, box.w, box.h, strored_origin[entity->GetIndex()]))
				continue;

			if (vars.visuals.box)
			{
				auto clr = dormant ? color_t(255, 255, 255, alpha - 50) : color_t(
					vars.visuals.box_color[0],
					vars.visuals.box_color[1],
					vars.visuals.box_color[2],
					alpha - 50);

				Drawing::DrawOutlinedRect(box.x, box.y, box.w, box.h, clr);
				Drawing::DrawOutlinedRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, color_t(10, 10, 10, (alpha * 0.8f)));
				Drawing::DrawOutlinedRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, color_t(10, 10, 10, (alpha * 0.8f)));
			}

			int count = 0;
			if (vars.visuals.name)
			{
				auto text_size = Drawing::GetTextSize(fonts::esp_name, entity->GetName().c_str());

				int r = vars.visuals.name_color[0],
					g = vars.visuals.name_color[1],
					b = vars.visuals.name_color[2],
					a = vars.visuals.name_color[3] * (alpha / 255.f);

				auto clr = dormant ? color_t(255, 255, 255, alpha) : color_t(r, g, b, a);

				Drawing::DrawString(fonts::esp_name, box.x + box.w / 2 - text_size.right / 2, box.y - 13, clr, FONT_LEFT, entity->GetName().c_str());
			}
			if (!dormant) {
				auto record = g_Animfix->get_latest_animation(entity);
				if (record.has_value() && record.value()->player) {
					auto clr = color_t(
						vars.visuals.skeleton_color[0],
						vars.visuals.skeleton_color[1],
						vars.visuals.skeleton_color[2],
						alpha - 50);

					if (vars.visuals.skeleton) {
						studiohdr_t* pStudioHdr = interfaces.models.model_info->GetStudioModel(entity->GetModel());

						if (!pStudioHdr)
							return;

						Vector vParent, vChild, sParent, sChild;

						for (int j = 0; j < pStudioHdr->numbones; j++)
						{
							mstudiobone_t* pBone = pStudioHdr->pBone(j);

							if (pBone && (pBone->flags & 0x100) && (pBone->parent != -1))
							{
								vChild = entity->GetBonePos(record.value()->bones, j);
								vParent = entity->GetBonePos(record.value()->bones, pBone->parent);

								if (Math::WorldToScreen(vParent, sParent) && Math::WorldToScreen(vChild, sChild))
								{
									Drawing::DrawLine(sParent[0], sParent[1], sChild[0], sChild[1], clr);
								}
							}
						}
					}
				}
			}

			if (vars.visuals.healthbar)
			{
				int hp = entity->GetHealth();

				if (hp > 100)
					hp = 100;

				int hp_percent = box.h - (int)((box.h * hp) / 100);

				int width = (box.w * (hp / 100.f));

				int red = 255 - (hp * 2.55);
				int green = hp * 2.55;

				char hps[10] = "";

				sprintf_s(hps, "%i", hp);

				auto text_size = Drawing::GetTextSize(fonts::esp_info, hps);

				auto clr = dormant ? color_t(200, 200, 200, alpha) : color_t(255, 255, 255, alpha);

				auto hp_color = dormant ? color_t(200, 200, 200, alpha) : vars.visuals.override_hp ? color_t(
					vars.visuals.hp_color[0],
					vars.visuals.hp_color[1],
					vars.visuals.hp_color[2],
					vars.visuals.hp_color[3] * (alpha / 255.f)) : color_t(red, green, 0, alpha);

				Drawing::DrawRect(box.x - 6, box.y - 1, 4, box.h + 2, color_t(10, 10, 10, alpha * 0.49f));
				Drawing::DrawRect(box.x - 5, box.y + hp_percent, 2, box.h - hp_percent, hp_color);

				if (hp <= 99) // draws hp count at the same pos as top of bar
					Drawing::DrawString(fonts::esp_info, box.x - text_size.left, box.y + hp_percent - 1,
						color_t(255, 255, 255, alpha), FONT_LEFT, hps);

			}
			if (weapon)
			{

				Vector dist = csgo->local->GetOrigin() - entity->GetOrigin();
				float flDist = sqrtf(dist.Length());

				if (flDist < 1.f)
					flDist = 1.f;
				if (weapon->IsGun())
				{
					auto ammo = weapon->GetAmmo(false);
					auto max_ammo = weapon->GetAmmo(true);
					if (vars.visuals.ammo)
					{
						auto clr = color_t(
							vars.visuals.ammo_color[0],
							vars.visuals.ammo_color[1],
							vars.visuals.ammo_color[2],
							vars.visuals.ammo_color[3] * (alpha / 255.f));

						int hp_percent = box.w - (int)((box.w * ammo) / 100);

						int width = (box.w * (ammo / float(max_ammo)));

						char ammostr[10];
						sprintf_s(ammostr, "%d", ammo);

						const auto text_size = Drawing::GetTextSize(fonts::esp_info, ammostr);

						Drawing::DrawRect(box.x, box.y + 2 + box.h, box.w + 1, 4, color_t(80, 80, 80, alpha * 0.49f));
						Drawing::DrawOutlinedRect(box.x, box.y + 2 + box.h, box.w + 1, 4, color_t(10, 10, 10, (alpha * 0.8f)));
						Drawing::DrawRect(box.x + 1, box.y + 3 + box.h, width - 1, 2, clr);
						count++;
					}
				}
				std::stringstream ft;
				ft << std::to_string((int)flDist) << "FT";
				auto gaysex = Drawing::GetTextSize(fonts::esp_info, ft.str().c_str());
				Drawing::DrawString(fonts::esp_info, box.x + box.w / 2, box.y + 1 + box.h + (count++ * 3.f), vars.visuals.ft_color, FONT_CENTER, ft.str().c_str());
				count++;
				if (vars.visuals.weapon)
				{
					auto clr = vars.visuals.weapon_color;
					auto weap_info = entity->GetWeapon()->GetCSWpnData();
					if (vars.visuals.weapon)
					{
						auto wpn_name_size = Drawing::GetTextSize(fonts::weapon_fonts, entity->GetWeapon()->get_weapon_name());
						Drawing::DrawString(fonts::weapon_fonts, box.x + box.w / 2 - wpn_name_size.right / 2, box.y + 1 + box.h + (count++ * 6.f), clr, FONT_LEFT, entity->GetWeapon()->get_weapon_name());
					}
					if (vars.visuals.weapon_icon)
					{
						auto wpn_name_sizei = Drawing::GetTextSize(fonts::esp_icon, entity->GetWeapon()->get_weapon_name_icon());
						Drawing::DrawString(fonts::esp_icon, box.x + box.w / 2 - wpn_name_sizei.right / 2, box.y + 1 + box.h + (count++ * 8.f), clr, FONT_LEFT, entity->GetWeapon()->get_weapon_name_icon());
					}
				}
			}
		}
	}
	for (auto entity : players)
	{
		Nades(entity);
	}
}
void CVisuals::Penetration_crosshair()
{
	if (csgo->local && csgo->local->isAlive() && interfaces.engine->IsConnected() && interfaces.engine->IsInGame())
	{
		auto weaponData = csgo->local->GetWeapon()->GetCSWpnData();
		if (!weaponData)
			return;

		float length = weaponData->m_flRange;
		CGameTrace tr;
		Ray_t r;
		CTraceFilter filter;
		filter.pSkip = csgo->local;

		Vector start;
		csgo->local->ShootPos(&start);
		Vector viewangles;
		interfaces.engine->GetViewAngles(viewangles);
		Vector fw;

		Math::AngleVectors(viewangles, fw);
		fw.Normalize();

		Vector end = start + fw * length;

		r.init(start, end);

		interfaces.trace->TraceRay(r, MASK_SHOT | CONTENTS_GRATE, 0, &tr);

		auto info = g_AutoWall.Think(tr.endpos, nullptr);
		bool can_penetrace = false;

		if (info.m_damage >= 1)
		{
			can_penetrace = true;
		}
		else
		{
			can_penetrace = false;
		}

		int w, h = 0;
		interfaces.engine->GetScreenSize(w, h);

		Drawing::DrawBox(w / 2 - 2, h / 2 - 2, 4, 4, can_penetrace ? color_t(0, 255, 0, 255) : color_t(255, 0, 0, 255));
	}
}
bool CVisuals::IsValidSound(SndInfo_t& sound) {
	// We don't want the sound to keep following client's predicted origin.
	for (int iter = 0; iter < m_utlvecSoundBuffer.Count(); iter++)
	{
		SndInfo_t& cached_sound = m_utlvecSoundBuffer[iter];
		if (cached_sound.m_nGuid == sound.m_nGuid)
		{
			return false;
		}
	}

	return true;
}

void CVisuals::OnDormant() {
	if (!interfaces.engine->IsInGame() || !csgo->local || !csgo->local->isAlive() || csgo->disable_dt)
		return;

	m_utlCurSoundList.RemoveAll();
	interfaces.engine_sound->GetActiveSounds(m_utlCurSoundList);

	// No active sounds.
	if (!m_utlCurSoundList.Count())
		return;

	// Accumulate sounds for esp correction
	for (int iter = 0; iter < m_utlCurSoundList.Count(); iter++)
	{
		SndInfo_t& sound = m_utlCurSoundList[iter];
		if (sound.m_nSoundSource == 0 || // World
			sound.m_nSoundSource > 64)   // Most likely invalid
			continue;

		auto player = interfaces.ent_list->GetClientEntity(sound.m_nSoundSource);

		if (!player || player == csgo->local || sound.m_pOrigin->IsZero())
			continue;

		if (!IsValidSound(sound))
			continue;

		AdjustDormant(player, sound);

		csgo->LastSeenTime[player->GetIndex()] = csgo->get_absolute_time();

		m_cSoundPlayers[sound.m_nSoundSource - 1].Override(sound);
	}

	m_utlvecSoundBuffer = m_utlCurSoundList;
}

void CVisuals::AdjustDormant(IBasePlayer* player, SndInfo_t& sound)
{
	Vector src3D, dst3D;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = player;
	src3D = (*sound.m_pOrigin) + Vector(0, 0, 1); // So they dont dig into ground incase shit happens /shrug
	dst3D = src3D - Vector(0, 0, 100);
	ray.Init(src3D, dst3D);

	interfaces.trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	if (tr.allsolid)
	{
		m_cSoundPlayers[sound.m_nSoundSource - 1].m_iReceiveTime = -1;
	}

	*sound.m_pOrigin = ((tr.fraction < 0.97) ? tr.endpos : *sound.m_pOrigin);
	m_cSoundPlayers[sound.m_nSoundSource - 1].m_nFlags = player->GetFlags();
	m_cSoundPlayers[sound.m_nSoundSource - 1].m_nFlags |= (tr.fraction < 0.50f ? FL_DUCKING : 0) | (tr.fraction != 1 ? FL_ONGROUND : 0);   // Turn flags on
	m_cSoundPlayers[sound.m_nSoundSource - 1].m_nFlags &= (tr.fraction > 0.50f ? ~FL_DUCKING : 0) | (tr.fraction == 1 ? ~FL_ONGROUND : 0); // Turn flags off
}

void CVisuals::RecordInfo()
{
	players.clear();
	for (auto i = 0; i < interfaces.global_vars->maxClients; i++)
	{
		IBasePlayer* entity = interfaces.ent_list->GetClientEntity(i);
		if (!entity)
			continue;

		players.push_back(entity);
	}
}

void CGrenadePrediction::Tick(int buttons)
{
	if (!vars.visuals.nadepred)
		return;
	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}
void CGrenadePrediction::View(CViewSetup* setup)
{
	if (!vars.visuals.nadepred)
		return;

	if (csgo->local && csgo->local->isAlive())
	{
		IBaseCombatWeapon* weapon = csgo->local->GetWeapon();

		if (weapon && weapon->IsNade() && act != ACT_NONE)
		{
			type = weapon->GetItemDefinitionIndex();
			Simulate(setup);
		}
		else
		{
			type = 0;
		}
	}
}
void CVisuals::Spectatorlist()
{
	if (!csgo->local && !csgo->local->isAlive())
		return;

	if (!vars.visuals.speclist)
		return;

	std::vector< std::string > spectators{("")};
	int hq = 12;

	for (int i{ 1 }; i <= interfaces.global_vars->maxClients; ++i) {
		IBasePlayer* player = interfaces.ent_list->GetClientEntity(i);
		if (!player)
			continue;

		if (player->IsDormant())
			continue;

		if (player->isAlive())
			continue;

		if (player->GetObserverTargetHandle() == csgo->local)
			continue;

		player_info_t info;
		if (!interfaces.engine->GetPlayerInfo(i, &info))
			continue;

		spectators.push_back(std::string(info.name).substr(0, 24));
	}

	size_t total_size = spectators.size() * (hq - 1);
	int w, h = 0;
	interfaces.engine->GetScreenSize(w, h);

	for (size_t i{ }; i < spectators.size(); ++i) {
		const std::string& name = spectators[i];

		Drawing::DrawString(fonts::esp_name, w - 10, 0 + (i * (hq - 1)), color_t(255, 255, 255, 179), FONT_RIGHT, name.c_str());
	}
}

void CGrenadePrediction::Paint()
{
	if (!vars.visuals.nadepred)
		return;

	if ((type) && !(path.empty()))
	{
		Vector nadeStart, nadeEnd;
		Vector nadeStart1, nadeEnd1;

		Vector prev = path[0];

		auto draw_3d_dotted_circle = [](Vector position, float points, float radius) {
			float step = pi * 2.0f / points;
			for (float a = 0; a < pi * 2.0f; a += step) {
				Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);

				Vector start2d;
				if (Math::WorldToScreen(start, start2d))
					Drawing::DrawLine(start2d.x, start2d.y, start2d.x + 1, start2d.y + 1, color_t(255, 255, 255));
			}
		};

		for (auto it = path.begin(), end = path.end(); it != end; ++it)
		{
			if (Math::WorldToScreen2(prev, nadeStart) && Math::WorldToScreen2(*it, nadeEnd))
			{
				Drawing::DrawLine((int)nadeStart.x, (int)nadeStart.y, (int)nadeEnd.x, (int)nadeEnd.y, vars.visuals.nadepred_color);
			}
			prev = *it;
		}
		Drawing::DrawFilledCircle((int)nadeEnd.x, (int)nadeEnd.y, 3, 20, vars.visuals.nadepred_color);
	}
}
const auto PIRAD = 0.01745329251f;
void angle_vectors2(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sr, sp, sy, cr, cp, cy;

	sp = static_cast<float>(sin(double(angles.x) * PIRAD));
	cp = static_cast<float>(cos(double(angles.x) * PIRAD));
	sy = static_cast<float>(sin(double(angles.y) * PIRAD));
	cy = static_cast<float>(cos(double(angles.y) * PIRAD));
	sr = static_cast<float>(sin(double(angles.z) * PIRAD));
	cr = static_cast<float>(cos(double(angles.z) * PIRAD));

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}
void CGrenadePrediction::Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles)
{
	if (!vars.visuals.nadepred)
		return;

	Vector angThrow = viewangles;
	float pitch = angThrow.x;

	if (pitch <= 90.0f)
	{
		if (pitch < -90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	angle_vectors2(angThrow, &vForward, &vRight, &vUp); //angThrow.ToVector(vForward, vRight, vUp);

	vecSrc = csgo->unpred_eyepos;
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f; //vecDest.MultAdd(vForward, 22.0f);

	TraceHull(vecSrc, vecDest, tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = csgo->local->GetVelocity(); vecThrow *= 1.25f;
	vecThrow += vForward * flVel; //	vecThrow.MultAdd(vForward, flVel);
}

void CGrenadePrediction::Simulate(CViewSetup* setup)
{
	if (!vars.visuals.nadepred)
		return;
	Vector vecSrc, vecThrow;
	Vector angles; interfaces.engine->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = interfaces.global_vars->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;


	path.clear();
	for (unsigned int i = 0; i < path.max_size() - 1; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1)) break;

		// Reset the log timer every logstep OR we bounced
		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;
	}
	path.push_back(vecSrc);
}

int CGrenadePrediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{

	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	// Set new position
	vecSrc = tr.endpos;

	return result;
}


bool CGrenadePrediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (type)
	{
	case WEAPON_SMOKEGRENADE:
	case WEAPON_DECOY:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length2D() < 0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick % det_tick_mod);
		}
		return false;

	case WEAPON_MOLOTOV:
	case WEAPON_INC:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f)
			return true;
		// OR we've been flying for too long

	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick) * interval > 1.5f && !(tick % static_cast<int>(0.2f / interval));

	default:
		assert(false);
		return false;
	}
}

void CGrenadePrediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	if (!vars.visuals.nadepred)
		return;
	Ray_t ray;
	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));

	CTraceFilterWorldAndPropsOnly filter;
	//filter.SetIgnoreClass("BaseCSGrenadeProjectile");
	//filter.bShouldHitPlayers = false;

	interfaces.trace->TraceRay(ray, 0x200400B, &filter, &tr);
}

void CGrenadePrediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	if (!vars.visuals.nadepred)
		return;
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void CGrenadePrediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	if (!vars.visuals.nadepred)
		return;
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void CGrenadePrediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	if (!vars.visuals.nadepred)
		return;
	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity > 0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity < 0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr < flMinSpeedSqr)
	{
		//vecAbsVelocity.Zero();
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	// Stop if on ground
	if (tr.plane.normal.z > 0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval); //vecAbsVelocity.Mult((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
}

int CGrenadePrediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}

void CVisuals::NightMode()
{
	static auto load_named_sky = reinterpret_cast<void(__fastcall*)(const char*)>(
		csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
			hs::load_named_sky::s().c_str()));

	static auto sv_skyname = interfaces.cvars->FindVar(hs::sv_skyname::s().c_str());
	sv_skyname->m_fnChangeCallbacks.m_Size = 0;

	static auto r_3dsky = interfaces.cvars->FindVar(hs::r_3dsky::s().c_str());
	r_3dsky->m_fnChangeCallbacks.m_Size = 0;
	r_3dsky->SetValue(0);

	static auto backup = sv_skyname->GetString();

	const auto reset = [&]()
	{
		load_named_sky(hs::sky_dust::s().c_str());

		for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
		{
			auto mat = interfaces.material_system->GetMaterial(i);
			if (!mat)
				continue;

			if (mat->IsErrorMaterial())
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();


			if (strstr(tex_name, hs::World::s().c_str()) || strstr(tex_name, hs::StaticProp::s().c_str()) || strstr(tex_name, hs::SkyBox::s().c_str()))
			{
				mat->ColorModulate(1.f, 1.f, 1.f);
				mat->AlphaModulate(1.f);
			}
		}
	};

	static bool Reset = false;
	static bool Reset2 = false;

	if (vars.visuals.nightmode)
	{
		Reset = false;

		const char* hay = "";
		if (hay != vars.visuals.skybox_name.c_str())
		{
			load_named_sky(vars.visuals.skybox_name.c_str());
			hay = vars.visuals.skybox_name.c_str();
		}

		static float OldClr[3];
		static float OldClr1;
		static float OldClr2[3];

		if (csgo->mapChanged || !Reset2 || OldClr[0] != vars.visuals.nightmode_color[0]
			|| OldClr[1] != vars.visuals.nightmode_color[1]
			|| OldClr[2] != vars.visuals.nightmode_color[2])
		{
			for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
			{
				auto mat = interfaces.material_system->GetMaterial(i);
				if (!mat)
					continue;

				auto tex_name = mat->GetTextureGroupName();

				if (strstr(tex_name, hs::World::s().c_str()))
				{
					mat->ColorModulate(vars.visuals.nightmode_color[0] / 255.f,
						vars.visuals.nightmode_color[1] / 255.f, vars.visuals.nightmode_color[2] / 255.f);
				}
			}

			OldClr[0] = vars.visuals.nightmode_color[0];
			OldClr[1] = vars.visuals.nightmode_color[1];
			OldClr[2] = vars.visuals.nightmode_color[2];
			csgo->mapChanged = false;
			Reset2 = true;
		}

		if (csgo->mapChanged || !Reset2 || OldClr2[0] != vars.visuals.nightmode_skybox_color[0]
			|| OldClr2[1] != vars.visuals.nightmode_skybox_color[1]
			|| OldClr2[2] != vars.visuals.nightmode_skybox_color[2])
		{
			for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
			{
				auto mat = interfaces.material_system->GetMaterial(i);
				if (!mat)
					continue;

				auto tex_name = mat->GetTextureGroupName();

				if (strstr(tex_name, hs::SkyBox::s().c_str()))
				{
					mat->ColorModulate(vars.visuals.nightmode_skybox_color[0] / 255.f,
						vars.visuals.nightmode_skybox_color[1] / 255.f, vars.visuals.nightmode_skybox_color[2] / 255.f);
				}
			}

			OldClr2[0] = vars.visuals.nightmode_skybox_color[0];
			OldClr2[1] = vars.visuals.nightmode_skybox_color[1];
			OldClr2[2] = vars.visuals.nightmode_skybox_color[2];
			csgo->mapChanged = false;
			Reset2 = true;
		}
	}
	else
	{
		Reset2 = false;

		if (!Reset)
		{
			reset();
			Reset = true;
		}
		csgo->mapChanged = false;
	}
}