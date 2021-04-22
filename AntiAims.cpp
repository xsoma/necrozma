#include "Hooks.h"
#include "AntiAims.h"
#include "Ragebot.h"
bool CanDT() {
	int idx = csgo->weapon->GetItemDefinitionIndex();
	return csgo->local->isAlive() && csgo->weapon->DTable()
		&& csgo->client_state->iChokedCommands <= 1
		&& idx != WEAPON_REVOLVER
		&& idx != WEAPON_ZEUSX27
		&& vars.ragebot.double_tap->active && !csgo->fake_duck;
}
bool CanHD() {
	int idx = csgo->weapon->GetItemDefinitionIndex();
	return csgo->local->isAlive() && csgo->weapon->HTable()
		&& csgo->client_state->iChokedCommands <= 1
		&& idx != WEAPON_REVOLVER
		&& idx != WEAPON_ZEUSX27 && vars.ragebot.hide_shots->active && !csgo->fake_duck;
}
bool CanHS() {
	return
		false;
}

void CMAntiAim::Fakelag(bool& send_packet)
{
	if (!vars.antiaim.enable)
		return;

	bool dt = CanDT();
	bool hs = CanHD();

	bool exp = dt || hs;

	if (interfaces.engine->IsVoiceRecording()) {
		csgo->max_fakelag_choke = 1;
		return;
	}

	if (dt && did_shot)
		return;

	if ((csgo->cmd->buttons & IN_ATTACK) && !vars.antiaim.fakelag_onshot) {
		send_packet = true;
		csgo->max_fakelag_choke = exp ? 1 : 2;
		return;
	}

	if (exp)
	{
		send_packet = exp ? csgo->client_state->iChokedCommands >= 1 : csgo->client_state->iChokedCommands >= 1;

		csgo->max_fakelag_choke = 1;
		return;
	}

	auto animstate = csgo->local->GetPlayerAnimState();
	if (!animstate)
		return;

	int tick_to_choke = 1;
	csgo->max_fakelag_choke = 16;

	static Vector oldOrigin;

	if (!(csgo->local->GetFlags() & FL_ONGROUND))
	{
		csgo->canDrawLC = true;
	}
	else {
		csgo->canDrawLC = false;
		csgo->canBreakLC = false;
	}

	if (vars.ragebot.double_tap->active)
		csgo->canDrawLC = false;

	if (vars.antiaim.fakeduck->active)
	{
		tick_to_choke = 14;
	}
	else
	{
		if (vars.antiaim.fakelag < 0)
			tick_to_choke = 2;
		else
		{
			switch (vars.antiaim.fakelag)
			{
			case 0:
				tick_to_choke = 1;
				break;
			case 1:
				tick_to_choke = vars.antiaim.fakelagfactor;
				break;
			case 2:
			{
				int factor = vars.antiaim.fakelagvariance;
				if (factor == 0 || factor > 15)
					factor = 15;

				if (csgo->cmd->command_number % factor < vars.antiaim.fakelagfactor)
					tick_to_choke = min(vars.antiaim.fakelagfactor, csgo->max_fakelag_choke);
				else
					tick_to_choke = 1;
			}
			break;
			case 3:
				int i = Math::RandomInt(1, 16);

				tick_to_choke = i;
				break;
			}
		}

		if (tick_to_choke < 1)
			tick_to_choke = 1;

		if (tick_to_choke > csgo->max_fakelag_choke)
			tick_to_choke = csgo->max_fakelag_choke;
	}
	send_packet = csgo->client_state->iChokedCommands >= tick_to_choke;

	static Vector sent_origin = Vector();

	if (csgo->canDrawLC) {
		if (send_packet)
			sent_origin = csgo->local->GetAbsOrigin();

		if ((sent_origin - oldOrigin).LengthSqr() > 4096.f) {
			csgo->canBreakLC = true;
		}
		else
			csgo->canBreakLC = false;

		if (send_packet)
			oldOrigin = csgo->local->GetAbsOrigin();
	}
}

void CMAntiAim::Pitch()
{
	switch (vars.antiaim.pitch)
	{
	case 0: return;
	case 1: csgo->cmd->viewangles.x = 89; break;
	case 2: csgo->cmd->viewangles.x = 89; break;
	case 3: csgo->cmd->viewangles.x = -89; break;
	}
}
void CMAntiAim::Sidemove() {
	if (!csgo->should_sidemove)
		return;

	if (csgo->local->GetWeapon()->GetItemDefinitionIndex() == WEAPONTYPE_GRENADE)
		return;

	float sideAmount = 2 * ((csgo->cmd->buttons & IN_DUCK || csgo->cmd->buttons & IN_WALK) ? 3.f : 0.505f);
	if (csgo->local->GetVelocity().Length2D() <= 0.f || std::fabs(csgo->local->GetVelocity().z) <= 100.f)
		csgo->cmd->sidemove += csgo->cmd->command_number % 2 ? sideAmount : -sideAmount;
}
bool UpdateLBY()
{
	static float next_lby_update_time = 0;
	const float current_time = interfaces.global_vars->interval_per_tick * csgo->local->GetTickBase();

	if (csgo->should_sidemove || csgo->local->GetVelocity().Length2D() > 10.f)
	{
		next_lby_update_time = current_time + TICKS_TO_TIME(1);
		return false;
	}
	else {
		if (next_lby_update_time < current_time)
		{
			next_lby_update_time = current_time + 1.1f;
			return true;
		}
	}
	return false;
}

float GetCurtime() {
	if (!csgo->local)
		return 0;
	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = csgo->local->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = csgo->cmd;
	float curtime = g_tick * interfaces.global_vars->interval_per_tick;
	return curtime;
}
float CMAntiAim::corrected_tickbase()
{
	CUserCmd* last_ucmd = nullptr;
	int corrected_tickbase = 0;

	corrected_tickbase = (!last_ucmd || last_ucmd->hasbeenpredicted) ? (float)csgo->local->GetTickBase() : corrected_tickbase++;
	last_ucmd = csgo->cmd;
	float corrected_curtime = corrected_tickbase * interfaces.global_vars->interval_per_tick;
	return corrected_curtime;
}
bool is_break()
{
	static float next_lby_update = 0.f;

	auto state = csgo->local->GetPlayerAnimState();

	float curtime = interfaces.global_vars->curtime;

	if (!state)
		return false;

	if (csgo->local->GetAbsVelocity().Length2D() > 0.1f || fabs(csgo->local->GetAbsVelocity().z) > 100.f)
	{
		next_lby_update = curtime + 0.22f;
	}
	if (next_lby_update < curtime)
	{
		next_lby_update = curtime + 1.1f;
		return true;
	}
	return false;
}
void CMAntiAim::Yaw(bool& send_packet)
{
	int side = csgo->SwitchAA ? 1 : -1;
	//float delta = vars.antiaim.dsy_amount * 0.6f;
	bool update = UpdateLBY();
	static bool sFlip = false;
	if ((interfaces.global_vars->tickcount % 100) > 1 && (interfaces.global_vars->tickcount % 100) < 50)
		sFlip = true;
	else
		sFlip = false;
	side = csgo->SwitchAA ? 1 : -1;
	static bool sw = false;
	static float last_angle;
	if (vars.antiaim.desync_type == 1 && !CanDT() && csgo->local->GetVelocity().Length2D() < 10)
	{
		csgo->should_sidemove = false;
		if (update)
		{
			csgo->cmd->viewangles.y = (last_angle + (csgo->cmd->viewangles.y -= 58.f * side));
			csgo->send_packet = false;
			csgo->should_sidemove = false;
		}
	}
	else
		csgo->should_sidemove = true;

	if (vars.antiaim.attargets)
	{
		IBasePlayer* best_ent = nullptr;
		float best_dist = FLT_MAX;
		for (int i = 1; i < 65; i++)
		{
			auto ent = interfaces.ent_list->GetClientEntity(i);
			if (!ent)
				continue;
			if (
				!ent->isAlive()
				|| ent == csgo->local
				|| ent->GetTeam() == csgo->local->GetTeam()
				)
				continue;
			float dist = ent->GetOrigin().DistTo(csgo->local->GetOrigin());

			if (dist < best_dist)
			{
				best_ent = ent;
				best_dist = dist;
			}
		}

		if (best_ent)
			csgo->cmd->viewangles.y = Math::CalculateAngle(csgo->local->GetOrigin(), best_ent->GetOrigin()).y;

	}

	if (vars.antiaim.aa_override.enable)
	{
		static bool left, right, back;
		if (vars.antiaim.aa_override.left->active)
		{
			left = true;
			right = false;
			back = false;
		}
		else if (vars.antiaim.aa_override.right->active)
		{
			left = false;
			right = true;
			back = false;
		}
		else if (vars.antiaim.aa_override.back->active)
		{
			left = false;
			right = false;
			back = true;
		}

		if (left)
			csgo->cmd->viewangles.y -= 90;
		if (right)
			csgo->cmd->viewangles.y += 90;
	}

	csgo->cmd->viewangles.y -= 180.f;


	switch (vars.antiaim.desync_type)
	{
	case 0: {
		csgo->should_sidemove = true;
		if (!send_packet) {
			csgo->cmd->viewangles.y -= 58.f * side;
			if (csgo->g_flVelMod > 0.000000000001f);
			csgo->InLbyUpdate;
			if (csgo->next_lby_update);
			csgo->canBreakLC;
			if (csgo->canBreakLC) {
				csgo->next_lby_update = true;
			}
			if (csgo->local->GetDSYDelta())
				csgo->next_lby_update = false;
			csgo->cmd->viewangles.y -= 56.f * side;
			csgo->next_lby_update = true;
			csgo->InLbyUpdate = true;
			csgo->cmd->viewangles.y -= 58.f * side;
			csgo->InLbyUpdate = false;
			csgo->next_lby_update = true;
			if (GetKeyState(70)) {
				csgo->cmd->viewangles.y -= 54.f * side;
			}
		}
	}break;
	case 1: {
		csgo->should_sidemove = false;
		if (!send_packet) {
			csgo->cmd->viewangles.y -= 58.f * side;
			if (csgo->g_flVelMod > 0.000000000001f);
			csgo->InLbyUpdate;
			if (csgo->next_lby_update);
			csgo->canBreakLC;
			if (csgo->canBreakLC) {
				csgo->next_lby_update = true;
			}
			if (csgo->local->GetDSYDelta())
				csgo->next_lby_update = false;
			csgo->cmd->viewangles.y -= 56.f * side;
			csgo->next_lby_update = true;
			csgo->InLbyUpdate = true;
			csgo->cmd->viewangles.y -= 58.f * side;
			csgo->InLbyUpdate = false;
			csgo->next_lby_update = true;
			if (GetKeyState(70)) {
				csgo->cmd->viewangles.y -= 54.f * side;
			}
		}break;
	case 2: {
		csgo->should_sidemove = true;
		if (!send_packet)
		{
			static auto st = 0;
			st++;
			if (st < 2)
			{
				static auto j = false;
				csgo->cmd->viewangles.y += j ? vars.antiaim.jitter : -vars.antiaim.jitter * side;
				j = !j;
			}
			else
				st = 0;
		}
	case 3: {
		static auto alternate = false;


		int side = csgo->SwitchAA ? 1 : -1;

		if (!csgo->send_packet)
			csgo->cmd->viewangles.y = std::remainderf(csgo->cmd->viewangles.y + 120.f * side, 360.f);

		float sideAmount = 5.f * ((csgo->cmd->buttons & IN_DUCK || csgo->cmd->buttons & IN_WALK) ? 3.f : 1.f);

		if (csgo->local->GetVelocity().Length2D() <= 0.f || std::fabs(csgo->local->GetVelocity().z <= 100.f))
			csgo->cmd->sidemove += alternate ? sideAmount : -sideAmount;
		if (csgo->local->GetVelocity().Length2D() <= 0.f || csgo->next_lby_update != false);

		alternate = !alternate;
	}
	}break;
	}

	}
	csgo->delta = csgo->local->GetDSYDelta();
}


void CMAntiAim::Run(bool& send_packet)
{
	if (vars.antiaim.slowwalk->active || csgo->should_stop_slide)
	{
		const auto weapon = csgo->weapon;

		if (weapon) {

			const auto info = csgo->weapon->GetCSWpnData();

			float speed = 0.1f;
			if (info) {
				float max_speed = weapon->GetZoomLevel() == 0 ? info->m_flMaxSpeed : info->m_flMaxSpeedAlt;
				float ratio = max_speed / 250.0f;
				speed *= ratio;
			}


			csgo->cmd->forwardmove *= speed;
			csgo->cmd->sidemove *= speed;
		}
	}

	shouldAA = true;
	if (!vars.antiaim.enable) {
		shouldAA = false;
		return;
	}
	if (csgo->cmd->buttons & IN_USE)
	{
		shouldAA = false;
		return;
	}
	if (csgo->game_rules->IsFreezeTime()
		|| csgo->local->GetMoveType() == MOVETYPE_NOCLIP
		|| csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		shouldAA = false;
		return;
	}
	if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (Ragebot::Get().shot)
		{
			shouldAA = false;
			return;
		}
	}
	else
	{
		if (F::Shooting() || (csgo->cmd->buttons & IN_ATTACK))
		{
			shouldAA = false;
			return;
		}
		if (csgo->weapon->IsKnife()) {
			if ((csgo->cmd->buttons & IN_ATTACK || csgo->cmd->buttons & IN_ATTACK2) && Ragebot::Get().IsAbleToShoot())
			{
				shouldAA = false;
				return;
			}
		}
	}
	if (shouldAA)
	{
		Pitch();
		Yaw(send_packet);
	}
}