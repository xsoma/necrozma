
#include "Hooks.h"
#include "Autowall.h"

CAutoWall g_AutoWall;

ReturnInfo_t CAutoWall::Think(Vector pos, IBasePlayer* target, int specific_hitgroup)
{
	ReturnInfo_t return_info = ReturnInfo_t(-1, -1, 4, false, 0.f, nullptr);

	Vector start = csgo->local->GetEyePosition();

	FireBulletData_t fire_bullet_data;
	fire_bullet_data.m_start = start;
	fire_bullet_data.m_end = pos;
	fire_bullet_data.m_current_position = start;
	fire_bullet_data.m_thickness = 0.f;
	fire_bullet_data.m_penetration_count = 5;

	Math::AngleVectors(Math::CalculateAngle(start, pos), fire_bullet_data.m_direction);

	static const auto filter_simple = *reinterpret_cast<uint32_t*>(reinterpret_cast<uint32_t>(
		(void*)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
			hs::trace_filter::s().c_str())) + 0x3d);

	uint32_t dwFilter[4] = { filter_simple,
		reinterpret_cast<uint32_t>(csgo->local), 0, 0 };

	fire_bullet_data.m_filter = (ITraceFilter*)(dwFilter);

	auto weapon = csgo->local->GetWeapon();
	if (!weapon) return return_info;

	auto weapon_info = weapon->GetCSWpnData();
	if (!weapon_info) return return_info;

	float range = min(weapon_info->m_flRange, (start - pos).Length());

	pos = start + (fire_bullet_data.m_direction * range);
	fire_bullet_data.m_current_damage = weapon_info->m_iDamage;

	for (int i = 0; i < scanned_points.size(); i++) {
		if (pos == scanned_points[i]) {
			return_info.m_damage = scanned_damage[i];
			return return_info;
		}
	}

	while (fire_bullet_data.m_current_damage > 0 && fire_bullet_data.m_penetration_count > 0)
	{
		return_info.m_penetration_count = fire_bullet_data.m_penetration_count;

		TraceLine(fire_bullet_data.m_current_position, pos, MASK_SHOT | CONTENTS_GRATE, csgo->local, &fire_bullet_data.m_enter_trace);
		ClipTrace(fire_bullet_data.m_current_position, fire_bullet_data.m_current_position + (fire_bullet_data.m_direction * 40.f), target, MASK_SHOT | CONTENTS_GRATE, fire_bullet_data.m_filter, &fire_bullet_data.m_enter_trace);

		const float distance_traced = (fire_bullet_data.m_enter_trace.endpos - start).Length();
		fire_bullet_data.m_current_damage *= pow(weapon_info->m_flRangeModifier, (distance_traced / 500.f));

		if (fire_bullet_data.m_enter_trace.fraction == 1.f)
		{
			if (target && specific_hitgroup != -1)
			{
				ScaleDamage(target, weapon_info, specific_hitgroup, fire_bullet_data.m_current_damage);

				return_info.m_damage = fire_bullet_data.m_current_damage;
				return_info.m_hitgroup = specific_hitgroup;
				return_info.m_end = fire_bullet_data.m_enter_trace.endpos;
				return_info.m_hit_entity = target;
			}
			else
			{
				return_info.m_damage = fire_bullet_data.m_current_damage;
				return_info.m_hitgroup = -1;
				return_info.m_end = fire_bullet_data.m_enter_trace.endpos;
				return_info.m_hit_entity = nullptr;
			}

			break;
		}

		if (fire_bullet_data.m_enter_trace.hitgroup > 0 && fire_bullet_data.m_enter_trace.hitgroup <= 8)
		{
			if
				(
					(target && fire_bullet_data.m_enter_trace.m_pEnt != target)
					||
					(reinterpret_cast<IBasePlayer*>(fire_bullet_data.m_enter_trace.m_pEnt)->GetTeam() == csgo->local->GetTeam()))
			{
				return_info.m_damage = -1;
				return return_info;
			}

			if (specific_hitgroup != -1)
			{
				ScaleDamage(reinterpret_cast<IBasePlayer*>(fire_bullet_data.m_enter_trace.m_pEnt), weapon_info, specific_hitgroup, fire_bullet_data.m_current_damage);
			}
			else
			{
				ScaleDamage(reinterpret_cast<IBasePlayer*>(fire_bullet_data.m_enter_trace.m_pEnt), weapon_info, fire_bullet_data.m_enter_trace.hitgroup, fire_bullet_data.m_current_damage);
			}

			return_info.m_damage = fire_bullet_data.m_current_damage;
			return_info.m_hitgroup = fire_bullet_data.m_enter_trace.hitgroup;
			return_info.m_end = fire_bullet_data.m_enter_trace.endpos;
			return_info.m_hit_entity = fire_bullet_data.m_enter_trace.m_pEnt;

			break;
		}

		if (!HandleBulletPenetration(weapon_info, fire_bullet_data))
			break;

		return_info.m_did_penetrate_wall = true;
	}

	scanned_damage.push_back(return_info.m_damage);
	scanned_points.push_back(pos);

	return_info.m_penetration_count = fire_bullet_data.m_penetration_count;

	return return_info;
}

float CAutoWall::HitgroupDamage(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 0.5f;
	case HITGROUP_HEAD:
		return 2.0f;
	case HITGROUP_CHEST:
		return 0.5f;
	case HITGROUP_STOMACH:
		return 0.75f;
	case HITGROUP_LEFTARM:
		return 0.5f;
	case HITGROUP_RIGHTARM:
		return 0.5f;
	case HITGROUP_LEFTLEG:
		return 0.375f;
	case HITGROUP_RIGHTLEG:
		return 0.375f;
	case HITGROUP_GEAR:
		return 0.5f;
	default:
		return 1.0f;
	}

	return 1.0f;
}

void CAutoWall::ScaleDamage(IBasePlayer* e, CCSWeaponInfo* weapon_info, int hitgroup, float& current_damage)
{
	static auto mp_damage_scale_ct_head = interfaces.cvars->FindVar(hs::mp_damage_scale_ct_head::s().c_str());
	static auto mp_damage_scale_t_head = interfaces.cvars->FindVar(hs::mp_damage_scale_t_head::s().c_str());
	static auto mp_damage_scale_ct_body = interfaces.cvars->FindVar(hs::mp_damage_scale_ct_body::s().c_str());
	static auto mp_damage_scale_t_body = interfaces.cvars->FindVar(hs::mp_damage_scale_t_body::s().c_str());

	auto team = e->GetTeam();
	auto head_scale = team == 2 ? mp_damage_scale_ct_head->GetFloat() : mp_damage_scale_t_head->GetFloat();
	auto body_scale = team == 2 ? mp_damage_scale_ct_body->GetFloat() : mp_damage_scale_t_body->GetFloat();

	auto armor_heavy = e->HeavyArmor();
	auto armor_value = static_cast<float>(e->GetArmor());

	if (armor_heavy) head_scale *= 0.5f;

	// ref: CCSPlayer::TraceAttack

	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		current_damage = (current_damage * 4.f) * head_scale;
		break;
	case HITGROUP_CHEST:
	case 8:
		current_damage *= body_scale;
		break;
	case HITGROUP_STOMACH:
		current_damage = (current_damage * 1.25f) * body_scale;
		break;
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		current_damage *= body_scale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_damage = (current_damage * 0.75f) * body_scale;
		break;
	default:
		break;
	}

	static auto IsArmored = [](IBasePlayer* player, int hitgroup)
	{
		auto has_helmet = player->HasHelmet();
		auto armor_value = static_cast<float>(player->GetArmor());

		if (armor_value > 0.f)
		{
			switch (hitgroup)
			{
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
			case 8:
				return true;
				break;
			case HITGROUP_HEAD:
				return has_helmet || (bool)player->HeavyArmor();
				break;
			default:
				return (bool)player->HeavyArmor();
				break;
			}
		}

		return false;
	};

	if (IsArmored(e, hitgroup))
	{
		auto armor_scale = 1.f;
		auto armor_ratio = (weapon_info->m_flArmorRatio * 0.5f);
		auto armor_bonus_ratio = 0.5f;

		if (armor_heavy)
		{
			armor_ratio *= 0.2f;
			armor_bonus_ratio = 0.33f;
			armor_scale = 0.25f;
		}

		float new_damage = current_damage * armor_ratio;
		float estiminated_damage = (current_damage - (current_damage * armor_ratio)) * (armor_scale * armor_bonus_ratio);
		if (estiminated_damage > armor_value) new_damage = (current_damage - (armor_value / armor_bonus_ratio));

		current_damage = new_damage;
	}
}

bool CAutoWall::HandleBulletPenetration(CCSWeaponInfo* info, FireBulletData_t& data, bool extracheck, Vector point)
{
	CGameTrace trace_exit;
	surfacedata_t* enter_surface_data = interfaces.phys_props->GetSurfaceData(data.m_enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;

	float enter_surf_penetration_modifier = enter_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.18f;
	float compined_penetration_modifier = 0.f;
	bool solid_surf = ((data.m_enter_trace.contents >> 3) & CONTENTS_SOLID);
	bool light_surf = ((data.m_enter_trace.surface.flags >> 7) & SURF_LIGHT);

	if
		(
			data.m_penetration_count <= 0
			|| (!data.m_penetration_count && !light_surf && !solid_surf && enter_material != CHAR_TEX_GLASS && enter_material != CHAR_TEX_GRATE)
			|| info->m_flPenetration <= 0.f
			|| !TraceToExit(&data.m_enter_trace, data.m_enter_trace.endpos, data.m_direction, &trace_exit)
			&& !(interfaces.trace->GetPointContents(data.m_enter_trace.endpos, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL) & (MASK_SHOT_HULL | CONTENTS_HITBOX))
			)
	{
		return false;
	}

	surfacedata_t* exit_surface_data = interfaces.phys_props->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_modifier = exit_surface_data->game.flPenetrationModifier;

	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE) {
		compined_penetration_modifier = 3.f;
		final_damage_modifier = 0.08f;
	}
	else if (light_surf || solid_surf)
	{
		compined_penetration_modifier = 1.f;
		final_damage_modifier = 0.18f;
	}
	else {
		compined_penetration_modifier = (enter_surf_penetration_modifier + exit_surf_penetration_modifier) * 0.5f;
		final_damage_modifier = 0.18f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
			compined_penetration_modifier = 3.f;
		else if (exit_material == CHAR_TEX_PLASTIC)
			compined_penetration_modifier = 2.0f;
	}

	float thickness = (trace_exit.endpos - data.m_enter_trace.endpos).LengthSqr();
	float modifier = max(0.f, 1.f / compined_penetration_modifier);

	if (extracheck) {
		static auto VectortoVectorVisible = [&](Vector src, Vector point) -> bool {
			trace_t TraceInit;
			TraceLine(src, point, MASK_SOLID, csgo->local, &TraceInit);
			trace_t Trace;
			TraceLine(src, point, MASK_SOLID, TraceInit.m_pEnt, &Trace);

			if (Trace.fraction == 1.0f || TraceInit.fraction == 1.0f)
				return true;

			return false;
		};
		if (!VectortoVectorVisible(trace_exit.endpos, point))
			return false;
	}
	float lost_damage = max(((modifier * thickness) / 24.f) + ((data.m_current_damage * final_damage_modifier) + (max(3.75f / info->m_flPenetration, 0.f) * 3.f * modifier)), 0.f);

	if (lost_damage > data.m_current_damage)
		return false;

	if (lost_damage > 0.f)
		data.m_current_damage -= lost_damage;

	if (data.m_current_damage < 1.f)
		return false;

	data.m_current_position = trace_exit.endpos;
	data.m_penetration_count--;

	return true;
}

bool CAutoWall::TraceToExit(trace_t* enter_trace, Vector start, Vector dir, trace_t* exit_trace)
{
	Vector end;
	float distance = 0.f;
	signed int distance_check = 25;
	int first_contents = 0;

	do
	{
		distance += 3.5f;
		end = start + dir * distance;

		if (!first_contents) first_contents = interfaces.trace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE, NULL);

		int point_contents = interfaces.trace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE, NULL);

		if (!(point_contents & (MASK_SHOT_HULL | CONTENTS_HITBOX)) || point_contents & CONTENTS_HITBOX && point_contents != first_contents)
		{
			Vector new_end = end - (dir * 4.f);

			Ray_t ray;
			ray.Init(end, new_end);

			interfaces.trace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

			if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
			{
				TraceLine(end, start, MASK_SHOT_HULL | CONTENTS_HITBOX, exit_trace->m_pEnt, exit_trace);

				if (exit_trace->DidHit() && !exit_trace->startsolid) return true;

				continue;
			}

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				if (enter_trace->surface.flags & SURF_NODRAW || !(exit_trace->surface.flags & SURF_NODRAW)) {
					if (exit_trace->plane.normal.Dot(dir) <= 1.f)
						return true;

					continue;
				}

				if (IsBreakableEntity(enter_trace->m_pEnt)
					&& IsBreakableEntity(exit_trace->m_pEnt))
					return true;

				continue;
			}

			if (exit_trace->surface.flags & SURF_NODRAW)
			{
				if (IsBreakableEntity(enter_trace->m_pEnt)
					&& IsBreakableEntity(exit_trace->m_pEnt))
				{
					return true;
				}
				else if (!(enter_trace->surface.flags & SURF_NODRAW))
				{
					continue;
				}
			}

			if ((!enter_trace->m_pEnt
				|| enter_trace->m_pEnt->EntIndex() == 0)
				&& (IsBreakableEntity(enter_trace->m_pEnt)))
			{
				exit_trace = enter_trace;
				exit_trace->endpos = start + dir;
				return true;
			}

			continue;
		}

		distance_check--;
	} while (distance_check);

	return false;
}

void CAutoWall::TraceLine(Vector& start, Vector& end, unsigned int mask, IBasePlayer* ignore, trace_t* trace)
{
	Ray_t ray;
	ray.Init(start, end);

	CTraceFilter filter;
	filter.pSkip = ignore;

	interfaces.trace->TraceRay(ray, mask, &filter, trace);
}

void CAutoWall::ClipTrace(Vector& start, Vector& end, IBasePlayer* e, unsigned int mask, ITraceFilter* filter, trace_t* old_trace)
{
	if (!e)
		return;

	Vector mins = e->GetCollideable()->OBBMins(), maxs = e->GetCollideable()->OBBMaxs();

	Vector dir(end - start);
	dir.NormalizeInPlace();

	Vector center = (maxs + mins) / 2, pos(center + e->GetOrigin());

	Vector to = pos - start;
	float range_along = dir.Dot(to);

	float range;

	if (range_along < 0.f)
	{
		range = -to.Length();
	}
	else if (range_along > dir.Length())
	{
		range = -(pos - end).Length();
	}
	else
	{
		auto ray(pos - (dir * range_along + start));
		range = ray.Length();
	}

	if (range <= 60.f) //55.f 
	{
		trace_t trace;

		Ray_t ray;
		ray.Init(start, end);

		interfaces.trace->ClipRayToEntity(ray, mask, e, &trace);

		if (old_trace->fraction > trace.fraction) *old_trace = trace;
	}
}

bool CAutoWall::IsBreakableEntity(IBasePlayer* e)
{
	if (!e || !e->EntIndex())
		return false;

	static auto is_breakable_fn = reinterpret_cast<bool(__thiscall*)(IBasePlayer*)>(
		csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), hs::is_breakable::s().c_str()));

	const auto result = is_breakable_fn(e);
	auto class_id = e->GetClientClass()->m_ClassID;
	if (!result &&
		(class_id == 10 ||
			class_id == 31 ||
			(class_id == 11 && e->GetCollideable()->get_solid() == 1)))
		return true;

	return result;
}

bool CAutoWall::CanHitFloatingPoint(const Vector& point, const Vector& source) {
	static auto VectortoVectorVisible = [&](Vector src, Vector point) -> bool {
		trace_t TraceInit;
		TraceLine(src, point, MASK_SOLID, csgo->local, &TraceInit);
		trace_t Trace;
		TraceLine(src, point, MASK_SOLID, TraceInit.m_pEnt, &Trace);

		if (Trace.fraction == 1.0f || TraceInit.fraction == 1.0f)
			return true;

		return false;
	};
	FireBulletData_t data;
	data.m_start = source;
	data.m_filter = new ITraceFilter();
	data.m_filter->pSkip = csgo->local;
	Vector angles = Math::CalculateAngle(data.m_start, point);
	Math::AngleVectors(angles, &data.m_direction);
	Math::VectorNormalize(data.m_direction);

	data.m_penetration_count = 1;
	//data.trace_length = 0.0f;

	auto weaponData = csgo->weapon->GetCSWpnData();

	if (!weaponData)
		return false;

	data.m_current_damage = (float)weaponData->m_iDamage;
	//data.trace_length_remaining = weaponData->range - data.trace_length;
	Vector end = data.m_start + (data.m_direction * weaponData->m_flRange);
	TraceLine(data.m_start, end, MASK_SHOT | CONTENTS_HITBOX, csgo->local, &data.m_enter_trace);

	if (VectortoVectorVisible(data.m_start, point) || HandleBulletPenetration(weaponData, data, true, point))
		return true;

	return false;
}