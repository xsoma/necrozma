//#include "Hooks.h"
//#include "Legitbot.h"
//#include <DirectXMath.h>
//#include "Ragebot.h"
//
//float zRandomFloat(float a, float b) {
//	float random = ((float)rand()) / (float)RAND_MAX;
//	float diff = b - a;
//	float r = random * diff;
//	return a + r;
//}
//
//void Legitbot::Cleanup() {
//	shot_delay_time = 0;
//	shot_delay = false;
//	silent_enabled = false;
//	CurrentPunch = { 0,0,0 };
//	RCSLastPunch = { 0,0,0 };
//	is_delayed = false;
//	kill_delay_time = 0;
//	kill_delay = false;
//	target = NULL;
//}
//
//bool Legitbot::IsRcs() {
//	return csgo->local->GetShootsFired() >= (int)vars.legitbot.rcs_start;
//}
//
//void MathAngleVectors(const Vector& angles, Vector& forward)
//{
//	float sp, sy, cp, cy;
//
//	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
//	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
//
//	forward.x = cp * cy;
//	forward.y = cp * sy;
//	forward.z = -sp;
//}
//float GetRealDistanceFOV(float distance, Vector angle, Vector viewangles) {
//	Vector aimingAt;
//	MathAngleVectors(viewangles, aimingAt);
//	aimingAt *= distance;
//	Vector aimAt;
//	MathAngleVectors(angle, aimAt);
//	aimAt *= distance;
//	return aimingAt.DistTo(aimAt) / 5;
//}
//
//template<class T>
//void Normalize3(T& vec)
//{
//
//	for (auto i = 0; i < 2; i++) {
//		while (vec < -180.0f) vec += 360.0f;
//		while (vec > 180.0f) vec -= 360.0f;
//	}
//	vec[2] = 0.f;
//}
//void ClampAngles(Vector& angles)
//{
//	if (angles.x > 89.0f) angles.x = 89.0f;
//	else if (angles.x < -89.0f) angles.x = -89.0f;
//
//	if (angles.y > 180.0f) angles.y = 180.0f;
//	else if (angles.y < -180.0f) angles.y = -180.0f;
//
//	angles.z = 0;
//}
//void MathFixAngles(Vector& angles)
//{
//	Normalize3(angles);
//	ClampAngles(angles);
//}
//
//void Legitbot::AutoPistol()
//{
//	if (!csgo->local || !csgo->cmd || !csgo->weapon)
//		return;
//	/*if (csgo->weapon->isPistol() && csgo->weapon->GetItemDefinitionIndex() != WEAPON_REVOLVER)
//	{
//	if (csgo->cmd->buttons & IN_ATTACK)
//	{
//	float flNextAttack = csgo->weapon->NextPrimaryAttack(), flCurTime = (float)csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
//	if (flNextAttack > flCurTime)
//	{
//	csgo->cmd->buttons &= ~IN_ATTACK;
//	csgo->cmd->buttons |= IN_ATTACK;
//	csgo->cmd->buttons &= ~IN_ATTACK;
//	}
//	}
//	}*/
//	float flCurTime = (float)csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
//	if (csgo->weapon && csgo->weapon->isPistol() && csgo->weapon->NextPrimaryAttack() > flCurTime) {
//		if (csgo->weapon->GetItemDefinitionIndex() == weapon_revolver)
//			csgo->cmd->buttons &= ~IN_ATTACK2;
//		else
//			csgo->cmd->buttons &= ~IN_ATTACK;
//	}
//}
//
//float Legitbot::GetFovToPlayer(Vector viewAngle, Vector aimAngle) {
//	Vector delta = aimAngle - viewAngle;
//	MathFixAngles(delta);
//	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
//}
////--------------------------------------------------------------------------------
//bool Legitbot::IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos) {
//	static auto LineGoesThroughSmokeFn = (bool(*)(Vector vStartPos, Vector vEndPos))csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::is_Line_goes_through_smoke::s().c_str())].c_str());
//	return LineGoesThroughSmokeFn(vStartPos, vEndPos);
//}
////--------------------------------------------------------------------------------
//
//
//float IBaseCombatWeapon::ServerTime() {
//
//	static CUserCmd* lastcmd;
//	static int fixtickbase;
//
//
//	if (!lastcmd || lastcmd->hasbeenpredicted)
//		fixtickbase = csgo->local->GetTickBase();
//	else
//		fixtickbase++;
//
//	lastcmd = csgo->cmd;
//
//	return fixtickbase * interfaces.global_vars->interval_per_tick;
//}
//
//bool Legitbot::IsEnabled(CUserCmd* pCmd) {
//	if (!interfaces.engine->IsConnected() || !csgo->local || !csgo->local->isAlive()) {
//		return false;
//	}
//	csgo->weapon->GetItemDefinitionIndex();
//	auto weaponData = csgo->weapon->GetCSWpnData();
//
//	if (!weaponData)
//		return false;
//
//	auto weapontype = weaponData->m_iWeaponType;
//	if (!vars.legitbot.legitenable) {
//		return false;
//	}
//
//
//	if (csgo->weapon->InReload()) {
//		return false;
//	}
//
//	if (!Ragebot::Get().IsAbleToShoot())
//		return false;
//
//	return (vars.legitbot.legitkey->active);
//}
////--------------------------------------------------------------------------------
//float Legitbot::GetSmooth() {
//	float smooth = IsRcs() && vars.legitbot.rcs_smooth_enabled ? vars.legitbot.rcs_smooth : vars.legitbot.smooth;
//	return smooth;
//}
////--------------------------------------------------------------------------------
//void Legitbot::Smooth(Vector currentAngle, Vector aimAngle, Vector& angle) {
//	auto smooth_value = GetSmooth();
//	if (smooth_value <= 1) {
//		return;
//	}
//
//	Vector delta = aimAngle - currentAngle;
//	MathFixAngles(delta);
//
//	// ничего не трогать и не менять
//	if (vars.legitbot.smooth_type == 1) {
//		float deltaLength = fmaxf(sqrtf((delta.x * delta.x) + (delta.y * delta.y)), 0.01f);
//		delta *= (1.0f / deltaLength);
//
//		//RandomSeed(g_GlobalVars->tickcount);
//		float randomize = zRandomFloat(-0.1f, 0.1f);
//		smooth_value = fminf((interfaces.global_vars->interval_per_tick * 64.0f) / (randomize + smooth_value * 0.15f), deltaLength);
//	}
//	else {
//		smooth_value = (interfaces.global_vars->interval_per_tick * 64.0f) / smooth_value;
//	}
//
//	delta *= smooth_value;
//	angle = currentAngle + delta;
//	MathFixAngles(angle);
//}
//
//void SmoothAngle(Vector currentAngle, Vector& aimAngle, Vector angle, float factor)
//{
//	auto smooth_value = factor;
//	if (smooth_value <= 1) {
//		return;
//	}
//
//	Vector delta = aimAngle - currentAngle;
//	MathFixAngles(delta);
//
//	// ничего не трогать и не менять
//
//	smooth_value = (interfaces.global_vars->interval_per_tick * 64.0f) / smooth_value;
//
//
//	delta *= smooth_value;
//	angle = currentAngle + delta;
//	MathFixAngles(angle);
//}
//
//void Legitbot::RCS(Vector curang, Vector& angle, IBasePlayer* target, bool should_run) {
//	if (!vars.legitbot.rcs) {
//		RCSLastPunch.Init();
//		return;
//	}
//
//	if (vars.legitbot.rcs_x == 0 && vars.legitbot.rcs_y == 0) {
//		RCSLastPunch.Init();
//		return;
//	}
//
//	Vector punch = csgo->local->GetPunchAngle() * interfaces.cvars->FindVar("weapon_recoil_scale")->GetFloat();
//	for (auto i = 1; i <= interfaces.engine->GetMaxClients(); ++i) {
//		auto weapon = reinterpret_cast<IBaseCombatWeapon*>(interfaces.ent_list->GetClientEntity(i));
//
//		if (weapon && weapon->NextPrimaryAttack() > interfaces.global_vars->curtime) {
//			auto delta_angles = punch - RCSLastPunch;
//			auto delta = weapon->NextPrimaryAttack() - interfaces.global_vars->curtime;
//			if (delta >= interfaces.global_vars->interval_per_tick)
//				punch = RCSLastPunch + delta_angles / static_cast<float>(TIME_TO_TICKS(delta));
//		}
//	}
//
//	CurrentPunch = punch;
//	if (vars.legitbot.rcs_type == 1 && !should_run)
//		punch -= { RCSLastPunch.x, RCSLastPunch.y, 0.f };
//
//	RCSLastPunch = CurrentPunch;
//	if (!IsRcs()) {
//		return;
//	}
//
//	float pithcmult = vars.legitbot.rcs_x;
//
//	if (csgo->local->GetShootsFired() < 4 + vars.legitbot.rcs_start)
//		pithcmult += 10;
//	punch.x *= (pithcmult / 100.0f);
//	punch.y *= (vars.legitbot.rcs_y / 100.0f);
//
//	//SmoothAngle(curang -= punch , curang, angle, 10);
//	angle -= punch;
//	MathFixAngles(angle);
//}
//
//
//float Legitbot::GetFov() {
//	if (IsRcs() && vars.legitbot.rcs && vars.legitbot.rcs_fov_enabled) return vars.legitbot.rcs_fov;
//	if (!silent_enabled) return vars.legitbot.fov;
//	return vars.legitbot.silent_fov > vars.legitbot.fov ? vars.legitbot.silent_fov : vars.legitbot.fov;
//}
////--------------------------------------------------------------------------------
//Vector MathCalcAngle(const Vector& src, const Vector& dst)
//{
//	Vector vAngle;
//	Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
//	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
//
//	vAngle.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
//	vAngle.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
//	vAngle.z = 0.0f;
//
//	if (delta.x >= 0.0)
//		vAngle.y += 180.0f;
//
//	MathFixAngles(vAngle);
//
//	return vAngle;
//}
//IBasePlayer* Legitbot::GetClosestPlayer(CUserCmd* cmd, int& bestBone) {
//	Vector ang;
//	Vector eVecTarget;
//	Vector pVecTarget = csgo->local->GetEyePosition();
//	if (target && !kill_delay && vars.legitbot.kill_delay > 0 && target->IsNotTarget()) {
//		target = NULL;
//		shot_delay = false;
//		kill_delay = true;
//		kill_delay_time = (int)GetTickCount() + vars.legitbot.kill_delay;
//	}
//	if (kill_delay) {
//		if (kill_delay_time <= (int)GetTickCount()) kill_delay = false;
//		else return NULL;
//	}
//
//	IBasePlayer* player;
//	target = NULL;
//	int bestHealth = 100.f;
//	float bestFov = 9999.f;
//	float bestDamage = 0.f;
//	float bestBoneFov = 9999.f;
//	float bestDistance = 9999.f;
//	int health;
//	float fov;
//	float damage;
//	float distance;
//	int fromBone = vars.legitbot.aim_type == 1 ? 0 : vars.legitbot.hitbox;
//	int toBone = vars.legitbot.aim_type == 1 ? 7 : vars.legitbot.hitbox;
//	for (int i = 1; i < interfaces.engine->GetMaxClients(); ++i) {
//		damage = 0.f;
//		player = (IBasePlayer*)interfaces.ent_list->GetClientEntity(i);
//		if (player->IsNotTarget()) {
//			continue;
//		}
//		if (!vars.legitbot.deathmatch && player->GetTeam() == csgo->local->GetTeam()) {
//			continue;
//		}
//		for (int bone = fromBone; bone <= toBone; bone++) {
//			eVecTarget = player->GetHitboxPos(bone);
//			//Math::VectorAngles(eVecTarget - pVecTarget, ang);
//
//
//			ang = MathCalcAngle(csgo->local->GetEyePosition(), eVecTarget);
//
//			MathFixAngles(ang);
//
//			distance = csgo->local->GetEyePosition().DistTo(eVecTarget);
//			if (vars.legitbot.fov_type == 1)
//				fov = GetRealDistanceFOV(distance, cmd->viewangles + RCSLastPunch, ang);
//			else
//				fov = GetFovToPlayer(cmd->viewangles + RCSLastPunch, ang);
//
//			if (fov > GetFov())
//				continue;
//
//			/*if (!g::pLocalEntity->CanSeePlayer(player, eVecTarget)) {
//
//			if (!settings.autowall)
//			continue;
//
//			damage = g_AutoWall.CanHit(eVecTarget);
//			if (damage < settings.min_damage)
//			continue;
//
//			}
//			if ((settings.priority == 1 || settings.priority == 2) && damage == 0.f)
//			damage = g_AutoWall.CanHit(eVecTarget);*/
//
//			health = player->GetHealth() - damage;
//			// if (vars.legitbot.check_smoke && IsLineGoesThroughSmoke(pVecTarget, eVecTarget))
//			// continue;
//
//			bool OnGround = (csgo->local->GetFlags() & FL_ONGROUND);
//			if (vars.legitbot.check_jump && !OnGround)
//				continue;
//
//			if (vars.legitbot.aim_type == 1 && bestBoneFov < fov) {
//				continue;
//			}
//			bestBoneFov = fov;
//			if (
//				(vars.legitbot.priority == 0 && bestFov > fov) ||
//				(vars.legitbot.priority == 1 && bestHealth > health) ||
//				(vars.legitbot.priority == 2 && bestDamage < damage) ||
//				(vars.legitbot.priority == 3 && distance < bestDistance)
//				) {
//				bestBone = bone;
//				target = player;
//				bestFov = fov;
//				bestHealth = health;
//				bestDamage = damage;
//				bestDistance = distance;
//			}
//		}
//	}
//	return target;
//}
////--------------------------------------------------------------------------------
//bool Legitbot::IsNotSilent(float fov) {
//	return IsRcs() || !silent_enabled || (silent_enabled && fov > vars.legitbot.silent_fov);
//}
////--------------------------------------------------------------------------------
//
//bool IBasePlayer::IsNotTarget()
//{
//	return !this || IsDormant() || !IsPlayer() || this == csgo->local || !this->isAlive() || this->HasGunGameImmunity();
//}
//float MathClampYaw(float yaw)
//{
//	while (yaw > 180.f)
//		yaw -= 360.f;
//	while (yaw < -180.f)
//		yaw += 360.f;
//	return yaw;
//}
//void Legitbot::OnMove(CUserCmd* pCmd) {
//	if (!IsEnabled(pCmd)) {
//		if (csgo->local && interfaces.engine->IsInGame() && csgo->local->isAlive() && vars.legitbot.legitenable && vars.legitbot.rcs_type == 0) {
//			for (auto i = 1; i <= interfaces.engine->GetMaxClients(); ++i) {
//				auto pWeapon = reinterpret_cast<IBaseCombatWeapon*>(interfaces.ent_list->GetClientEntity(i));
//				if (pWeapon && !pWeapon->IsKnife() && !pWeapon->IsNade())
//				{
//					RCS(pCmd->viewangles, pCmd->viewangles, target, false);
//					MathFixAngles(pCmd->viewangles);
//					interfaces.engine->SetViewAngles(pCmd->viewangles);
//				}
//			}
//		}
//		else {
//			RCSLastPunch = { 0, 0, 0 };
//		}
//		is_delayed = false;
//		shot_delay = false;
//		kill_delay = false;
//		silent_enabled = vars.legitbot.silent && vars.legitbot.silent_fov > 0;
//		target = NULL;
//		return;
//	}
//	Vector oldAngle;
//	interfaces.engine->GetViewAngles(oldAngle);
//
//
//	//RandomSeed(pCmd->command_number);
//
//	auto weapon_data = csgo->weapon->GetCSWpnData();
//	if (!weapon_data)
//		return;
//
//	bool should_do_rcs = false;
//	Vector angles = pCmd->viewangles;
//	Vector current = angles;
//
//	float fov = 180.f;
//	if (!(vars.legitbot.check_flash && csgo->local->IsFlashed())) {
//		int bestBone = -1;
//		if (GetClosestPlayer(pCmd, bestBone)) {
//			//Math::VectorAngles(target->GetHitboxPos(bestBone) - g_LocalPlayer->GetEyePos(), angles);
//			Vector eVecTarget;
//			eVecTarget = target->GetHitboxPos(bestBone);
//			angles = MathCalcAngle(csgo->local->GetEyePosition(), eVecTarget);
//
//			if (vars.legitbot.smooth > 2 && vars.legitbot.humanize) {
//				float dist = MathClampYaw(angles.y - oldAngle.y);
//				if (dist > 180.0f) dist = 360.0f - dist;
//				eVecTarget += Vector(0, 0, vars.legitbot.curviness * dist);
//
//				angles = MathCalcAngle(csgo->local->GetEyePosition(), eVecTarget);
//			}
//
//
//
//
//			MathFixAngles(angles);
//
//			if (vars.legitbot.fov_type == 1)
//				fov = GetRealDistanceFOV(csgo->local->GetEyePosition().DistTo(eVecTarget), angles, pCmd->viewangles);
//			else
//				fov = GetFovToPlayer(pCmd->viewangles, angles);
//
//			should_do_rcs = true;
//
//			if (!vars.legitbot.silent && !is_delayed && !shot_delay && vars.legitbot.shot_delay > 0) {
//				is_delayed = true;
//				shot_delay = true;
//				shot_delay_time = GetTickCount() + vars.legitbot.shot_delay;
//			}
//
//			if (shot_delay && shot_delay_time <= GetTickCount()) {
//				shot_delay = false;
//			}
//
//			if (shot_delay) {
//				pCmd->buttons &= ~IN_ATTACK;
//			}
//
//
//			/*if (settings.autofire) {
//			if (!settings.autofire_key || InputSys::Get().IsKeyDown(settings.autofire_key)) {
//			if (!weapon_data->bFullAuto) {
//			if (pCmd->command_number % 2 == 0) {
//			pCmd->buttons &= ~IN_ATTACK;
//			}
//			else {
//			pCmd->buttons |= IN_ATTACK;
//			}
//			}
//			else {
//			pCmd->buttons |= IN_ATTACK;
//			}
//			}
//			}*/
//		}
//	}
//
//	if (IsNotSilent(fov) && (should_do_rcs || vars.legitbot.rcs_type == 0)) {
//		RCS(current, angles, target, should_do_rcs);
//	}
//	if (target && IsNotSilent(fov)) {
//		Smooth(current, angles, angles);
//	}
//
//
//	MathFixAngles(angles);
//	pCmd->viewangles = angles;
//
//	if (IsNotSilent(fov)) {
//		interfaces.engine->SetViewAngles(angles);
//	}
//
//	silent_enabled = false;
//	/*if (weapon->IsPistol() && settings.autopistol) {
//	float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
//	float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;
//	if (next_shot > 0) {
//	pCmd->buttons &= ~IN_ATTACK;
//	}
//	}*/
//}
