#include "Hooks.h"
#include "Bunnyhop.h"
#include "Legitbot.h"
#include "Ragebot.h"
#include "AntiAims.h"
#include "checksum_md5.h"
#include "Resolver.h"
#include "Knifebot.h"
#include <intrin.h>
#include "EnginePrediction.h"
#include "NetChannelhook.h"
#include "scripting/java loader/js_loader.h"
#include "Menu.h"

static bool in_cm = false;

float last_time_got_impact = 0;

float IBasePlayer::GetOldSimulationTime()
{
	static int m_flSimulationTime = netvars.GetOffset(hs::DT_BaseEntity::s().c_str(), hs::m_flSimulationTime::s().c_str()) + 4;
	return *(float*)((DWORD)this + m_flSimulationTime);
}

void UpdateSimulationTime() {
	for (int i = 1; i < 65; i++)
	{
		auto ent = interfaces.ent_list->GetClientEntity(i);
		if (!ent)
			continue;
		if (ent->IsDormant() || !ent->isAlive() || !ent->IsPlayer())
			continue;
		if (csgo->old_simtime[ent->GetIndex()] != ent->GetSimulationTime())
			csgo->old_simtime[ent->GetIndex()] = ent->GetSimulationTime();
	}
}

void DoClantag()
{
	auto SetClanTag = [](const char* tag, const char* name)
	{
		static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>((DWORD)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
			hs::set_clantag::s().c_str())));
		pSetClanTag(tag, name);
	};

	auto Marquee = [](std::string& clantag)
	{
		std::string temp = clantag;
		clantag.erase(0, 1);
		clantag += temp[0];
	};

	static bool Reset = false;

	if (vars.visuals.clantagspammer)
	{
		Reset = false;
		static float oldTime;
		float flCurTime = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
		static float flNextTimeUpdate = 0;
		static int iLastTime;

		float latency = interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
		if (int(interfaces.global_vars->curtime * 2 + latency) != iLastTime)
		{
			if (flNextTimeUpdate <= flCurTime || flNextTimeUpdate - flCurTime > 1.f)
			{
				auto clantag = "";

				switch (vars.misc.clantag)
				{
				case 0:
					switch (int(interfaces.global_vars->curtime * 3) % 28)
					{
					case 0: clantag = "          "; break;
					case 1: clantag = "         ga"; break;
					case 2: clantag = "        gam"; break;
					case 3: clantag = "       game"; break;
					case 4: clantag = "      games"; break;
					case 5: clantag = "     gamese"; break;
					case 6: clantag = "    gamesen"; break;
					case 7: clantag = "   gamesens"; break;
					case 8: clantag = "  gamesense"; break;
					case 9: clantag = " gamesense "; break;
					case 10: clantag = " gamesense "; break;
					case 11: clantag = " gamesense "; break;
					case 12: clantag = " gamesense "; break;
					case 13: clantag = " gamesense "; break;
					case 14: clantag = " gamesense "; break;
					case 15: clantag = " gamesense "; break;
					case 16: clantag = " gamesense "; break;
					case 17: clantag = " gamesense "; break;
					case 18: clantag = " gamesense "; break;
					case 19: clantag = "gamesense "; break;
					case 20: clantag = "amesense "; break;
					case 21: clantag = "mesense   "; break;
					case 22: clantag = "esense    "; break;
					case 23: clantag = "sense     "; break;
					case 24: clantag = "ense      "; break;
					case 25: clantag = "nse       "; break;
					case 26: clantag = "se        "; break;
					case 27: clantag = "e         "; break;
					}
					break;
				case 1:
					switch (int(interfaces.global_vars->curtime * 3) % 27)
					{
					case 0: clantag = "          "; break;
					case 1: clantag = "         su"; break;
					case 2: clantag = "        sup"; break;
					case 3: clantag = "       supr"; break;
					case 4: clantag = "      supre"; break;
					case 5: clantag = "     suprem"; break;
					case 6: clantag = "    suprema"; break;
					case 7: clantag = "   supremac"; break;
					case 8: clantag = "  supremacy"; break;
					case 9: clantag = " supremacy "; break;
					case 10: clantag = " supremacy "; break;
					case 11: clantag = " supremacy "; break;
					case 12: clantag = " supremacy "; break;
					case 13: clantag = " supremacy "; break;
					case 14: clantag = " supremacy "; break;
					case 15: clantag = " supremacy "; break;
					case 16: clantag = " supremacy "; break;
					case 17: clantag = " supremacy "; break;
					case 18: clantag = " supremacy "; break;
					case 19: clantag = "supremacy "; break;
					case 20: clantag = "upremacy "; break;
					case 21: clantag = "premacy   "; break;
					case 22: clantag = "remacy    "; break;
					case 23: clantag = "emacy     "; break;
					case 24: clantag = "macy      "; break;
					case 25: clantag = "acy       "; break;
					case 26: clantag = "cy        "; break;
					case 27: clantag = "y         "; break;
					}
					break;

				case 2:
					switch (int(interfaces.global_vars->curtime * 3) % 27)
					{
					case 0: clantag = "          "; break;
					case 1: clantag = "         sk"; break;
					case 2: clantag = "        ske"; break;
					case 3: clantag = "       skee"; break;
					case 4: clantag = "      skeet"; break;
					case 5: clantag = "     skeeti"; break;
					case 6: clantag = "    skeetid"; break;
					case 7: clantag = "   skeetidb"; break;
					case 8: clantag = "  skeetidb"; break;
					case 9: clantag = " skeetidb "; break;
					case 10: clantag = " skeetidb "; break;
					case 11: clantag = " skeetidb "; break;
					case 12: clantag = " skeetidb "; break;
					case 13: clantag = " skeetidb "; break;
					case 14: clantag = " skeetidb "; break;
					case 15: clantag = " skeetidb "; break;
					case 16: clantag = " skeetidb "; break;
					case 17: clantag = " skeetidb "; break;
					case 18: clantag = " skeetidb "; break;
					case 19: clantag = "skeetidb "; break;
					case 20: clantag = "keetidb "; break;
					case 21: clantag = "eetidb   "; break;
					case 22: clantag = "etidb    "; break;
					case 23: clantag = "tidb     "; break;
					case 24: clantag = "idb      "; break;
					case 25: clantag = "db       "; break;
					case 26: clantag = "b        "; break;
					case 27: clantag = "         "; break;
					}
					break;
				}
				SetClanTag(clantag, clantag);
			}
			iLastTime = int(interfaces.global_vars->curtime * 2 + latency);
		}
	}
	else
	{
		//if (!Reset)
		{
			static int iLastTime;

			float latency = interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
			if (int(interfaces.global_vars->curtime * 2 + latency) != iLastTime)
			{
				SetClanTag("", "");
				iLastTime = int(interfaces.global_vars->curtime * 2 + latency);
			}
			Reset = true;
		}
	}
}

namespace MoveFixer
{
	void Run(Vector original)
	{
		Vector real_viewangles;
		interfaces.engine->GetViewAngles(real_viewangles);

		Vector vecMove(csgo->cmd->forwardmove, csgo->cmd->sidemove, csgo->cmd->upmove);
		float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

		Vector angMove;
		Math::VectorAngles(vecMove, angMove);

		float yaw = DEG2RAD(csgo->cmd->viewangles.y - original.y + angMove.y);

		csgo->cmd->forwardmove = cos(yaw) * speed;
		csgo->cmd->sidemove = sin(yaw) * speed;

		csgo->cmd->buttons &= ~IN_RIGHT;
		csgo->cmd->buttons &= ~IN_MOVERIGHT;
		csgo->cmd->buttons &= ~IN_LEFT;
		csgo->cmd->buttons &= ~IN_MOVELEFT;
		csgo->cmd->buttons &= ~IN_FORWARD;
		csgo->cmd->buttons &= ~IN_BACK;

		if (csgo->cmd->forwardmove > 0.f)
			csgo->cmd->buttons |= IN_FORWARD;
		else if (csgo->cmd->forwardmove < 0.f)
			csgo->cmd->buttons |= IN_BACK;

		if (csgo->cmd->sidemove > 0.f)
		{
			csgo->cmd->buttons |= IN_RIGHT;
			csgo->cmd->buttons |= IN_MOVERIGHT;
		}
		else if (csgo->cmd->sidemove < 0.f)
		{
			csgo->cmd->buttons |= IN_LEFT;
			csgo->cmd->buttons |= IN_MOVELEFT;
		}
	}
}

void ProcessMissedShots()
{
	if (shot_snapshots.size() == 0)
		return;
	auto& snapshot = shot_snapshots.front();
	if (fabs((csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick) - snapshot.time) > 1.f)
	{
		if (snapshot.weapon_fire) {
			if (snapshot.bullet_impact)
			{
				string msg;
				msg += "Missed " + snapshot.entity->GetName();
				msg += "'s ";
				msg += snapshot.get_info(true);
				msg += " r: unregistered shot ";
				Msg(msg, color_t(124, 193, 21));
			}
			else
			{
				string msg;
				msg += "Missed " + snapshot.entity->GetName();
				msg += "'s ";
				msg += snapshot.get_info(true);
				msg += " r: unregistered shot ";
				Msg(msg, color_t(124, 193, 21));
			}
		}
		shot_snapshots.erase(shot_snapshots.begin());
		return;
	}

	if (snapshot.first_processed_time != -1.f) {
		if (snapshot.damage == -1 && snapshot.weapon_fire && snapshot.bullet_impact && snapshot.record->player) {
			string msg;
			msg += "Missed " + snapshot.entity->GetName();
			msg += "'s ";
			msg += snapshot.get_info(true);
			bool spread = false;
			if (snapshot.record->player) {
				const auto studio_model = interfaces.models.model_info->GetStudioModel(snapshot.record->player->GetModel());

				if (studio_model)
				{
					const auto angle = Math::CalculateAngle(snapshot.start, snapshot.impact);
					Vector forward;
					Math::AngleVectors(angle, forward);
					const auto end = snapshot.impact + forward * 2000.f;
					if (!CanHitHitbox(snapshot.start, end, snapshot.record, studio_model, snapshot.hitbox))
						spread = true;
				}
			}
			if (spread)
				msg += " r: spread ";
			else {
				csgo->missedshots[snapshot.entity->GetIndex()]++;
				msg += " r: r? ";
			}
			Msg(msg, color_t(124, 193, 21));
			shot_snapshots.erase(shot_snapshots.begin());
		}
	}
}
void Choker(int ticks)
{
	static int choked = 0;
	if (choked >= ticks)
	{
		csgo->send_packet = true;
		choked = 0;
	}
	else
	{
		csgo->send_packet = false;
		choked++;
	}
}
void FakeDuck(bool& send_packet)
{
	if (csgo->cmd->buttons & IN_JUMP || !(csgo->local->GetFlags() & FL_ONGROUND) || !vars.antiaim.enable) {
		csgo->fake_duck = false;
		return;
	}
	csgo->cmd->buttons |= IN_BULLRUSH;

	int choke = 14;

	if (vars.antiaim.fakeduck->active)
	{
		csgo->fake_duck = true;

		if (csgo->client_state->iChokedCommands <= choke / 2)
		{
			csgo->cmd->buttons &= ~IN_DUCK;
		}
		else
		{
			csgo->cmd->buttons |= IN_DUCK;
		}

		if (csgo->client_state->iChokedCommands < choke)
			send_packet = false;								// choke
		else
			send_packet = true;
	}
	else {
		csgo->fake_duck = false;
	}
}
void FixRevolver()
{
	if (!csgo->weapon)
		return;

	if (csgo->weapon->GetItemDefinitionIndex() == 64 && csgo->weapon_struct.work && csgo->weapon->NextSecondaryAttack() == FLT_MAX)
		csgo->weapon->NextSecondaryAttack() = csgo->weapon_struct.next_attack;
}
float GetCurTime(CUserCmd* ucmd) {
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = csgo->local->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * interfaces.global_vars->interval_per_tick;
	return curtime;
}
void __fastcall Hooked_RunCommand(void* ecx, void* edx, IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
{
	static auto RunCommand = g_pPredictHook->GetOriginal< RunCommandFn >(19);

	if (csgo->DoUnload || player == nullptr || csgo->local == nullptr || !csgo->local->isAlive())
		return RunCommand(ecx, player, ucmd, moveHelper);

	if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame()) {
		if (player == csgo->local) {
			if (1.f / interfaces.global_vars->interval_per_tick + csgo->unpdred_tick + 8 <= ucmd->tick_count)
			{
				ucmd->hasbeenpredicted = true;
				return;
			}

			float m_flVelModBackup = csgo->local->GetVelocityModifier();

			int m_nTickbase = csgo->local->GetTickBase();
			int m_flCurtime = interfaces.global_vars->curtime;

			if (ucmd->command_number == csgo->shot_cmd) {
				csgo->local->GetTickBasePtr() = CMAntiAim::Get().base_tick - csgo->m_nTickbaseShift + 1;
				interfaces.global_vars->curtime = TICKS_TO_TIME(csgo->local->GetTickBasePtr());
			}

			if (csgo->g_bOverrideVelMod && ucmd->command_number == csgo->client_state->nLastCommandAck + 1)
				csgo->local->GetVelocityModifier() = csgo->g_flVelMod;

			RunCommand(ecx, player, ucmd, moveHelper);

			if (ucmd->command_number == csgo->shot_cmd)
			{
				csgo->local->GetTickBasePtr() = m_nTickbase;
				interfaces.global_vars->curtime = m_flCurtime;
			}

			if (!csgo->g_bOverrideVelMod)
				csgo->local->GetVelocityModifier() = m_flVelModBackup;
		}
	}
	else
		RunCommand(ecx, player, ucmd, moveHelper);
}

bool __fastcall Hooked_InPrediction(CPrediction* prediction, uint32_t)
{
	static auto InPrediction = g_pPredictHook->GetOriginal< InPredictionFn >(14);

	if (csgo->DoUnload)
		return InPrediction(prediction);

	static const auto return_to_maintain_sequence_transitions = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::seq_transition::s().c_str());

	if (_ReturnAddress() == (uint32_t*)return_to_maintain_sequence_transitions)
		return false;

	return InPrediction(prediction);
}

void __fastcall Hooked_SetupMove(void* ecx, void* edx, IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper, void* pMoveData)
{
	using Fn = void(__thiscall*)(void*, IBasePlayer*, CUserCmd*, IMoveHelper*, void*);

	static auto SetupMove = g_pPredictHook->GetOriginal< Fn >(20);

	//if (&Prediction::Get().move_data != pMoveData && pMoveData != nullptr)
	//	memcpy(&Prediction::Get().move_data, pMoveData, 0x564);

	SetupMove(ecx, player, ucmd, moveHelper, pMoveData);
}

void chokelimit()
{
	static auto clMoveChokeClamp = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
		hs::choke_limit::s().c_str()) + 1;
	unsigned long protect = 0;
	VirtualProtect((void*)clMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &protect);
	*(std::uint32_t*)clMoveChokeClamp = 62;
	VirtualProtect((void*)clMoveChokeClamp, 4, protect, &protect);
}

void WriteUsercmd(bf_write* buf, CUserCmd* in, CUserCmd* out)
{
	static DWORD WriteUsercmdF = (DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::write_user_cmd::s().c_str());

	__asm
	{
		mov ecx, buf
		mov edx, in
		push out
		call WriteUsercmdF
		add esp, 4
	}
}

void __cdecl  Hooked_ClMove(float accumulated_extra_samples, bool bFinalTick)
{
	H::ClMove(accumulated_extra_samples, bFinalTick);
}

bool __fastcall Hooked_WriteUsercmdDeltaToBuffer(void* ecx, void*, int slot, bf_write* buf, int from, int to, bool isnewcommand)
{
	static auto ofunct = g_pClientHook->GetOriginal <WriteUsercmdDeltaToBufferFn>(24);

	static auto retn = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
		hs::write_user_cmd_retn::s().c_str());

	if ((_ReturnAddress()) != retn || csgo->DoUnload)
		return ofunct(ecx, slot, buf, from, to, isnewcommand);

	if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame()) {
		if (csgo->game_rules->IsFreezeTime())
			return ofunct(ecx, slot, buf, from, to, isnewcommand);

		if (csgo->m_nTickbaseShift <= 0 || csgo->client_state->iChokedCommands > 3)
			return ofunct(ecx, slot, buf, from, to, isnewcommand);

		if (from != -1)
			return true;
		uintptr_t stackbase;
		__asm mov stackbase, ebp;
		CCLCMsg_Move_t* msg = reinterpret_cast<CCLCMsg_Move_t*>(stackbase + 0xFCC);
		auto net_channel = *reinterpret_cast <INetChannel**> (reinterpret_cast <uintptr_t> (csgo->client_state) + 0x9C);
		int32_t new_commands = msg->m_nNewCommands;

		int32_t next_cmdnr = csgo->client_state->nLastOutgoingCommand + csgo->client_state->iChokedCommands + 1;
		int32_t total_new_commands = min(csgo->m_nTickbaseShift, 17);
		csgo->m_nTickbaseShift -= total_new_commands;

		from = -1;
		msg->m_nNewCommands = total_new_commands;
		msg->m_nBackupCommands = 0;

		for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
			if (!ofunct(ecx, slot, buf, from, to, true))
				return false;

			from = to;
		}

		CUserCmd* last_realCmd = interfaces.input->GetUserCmd(slot, from);
		CUserCmd fromCmd;

		if (last_realCmd)
			fromCmd = *last_realCmd;

		CUserCmd toCmd = fromCmd;
		toCmd.command_number++;
		toCmd.tick_count++;

		for (int i = new_commands; i <= total_new_commands; i++) {
			WriteUsercmd(buf, &toCmd, &fromCmd);
			fromCmd = toCmd;
			toCmd.command_number++;
			toCmd.tick_count++;
		}
		return true;

	}
	else
		return ofunct(ecx, slot, buf, from, to, isnewcommand);
}

bool CMAntiAim::can_exploit(int tickbase_shift)
{
	auto weapon = csgo->weapon;

	if (!weapon)
		return false;

	const auto info = (weapon->GetCSWpnData());

	if (!(info))
		return false;

	float curtime = TICKS_TO_TIME(csgo->local->GetTickBase() - tickbase_shift);

	if (curtime < csgo->local->m_flNextAttack())
		return false;

	if (curtime < weapon->NextPrimaryAttack())
		return false;

	return true;
}

void InitDoubleTap()
{
	static bool did_shift_before = false;
	static int double_tapped = 0;
	static int prev_shift_ticks = 0;
	static bool reset = false;
	static int shot_tick = 0;
	static int process_ticks = 0;
	CMAntiAim::Get().shot_dt = false;

	if (vars.ragebot.dttype == 1)
		vars.ragebot.dt_instant = true;
	else
		vars.ragebot.dt_instant = false;

	int fuck_tiks = csgo->weapon->isPistol() ? 11 : 17;

	if ((CanDT() || CanHD()) && !csgo->game_rules->IsFreezeTime())
	{
		if (vars.ragebot.double_tap->active)
		{
			prev_shift_ticks = 0;

			auto can_shift_shot = CMAntiAim::Get().can_exploit(fuck_tiks);
			auto can_shot = CMAntiAim::Get().can_exploit(abs(-1 - prev_shift_ticks));

			if (can_shift_shot || !can_shot && !did_shift_before)
			{
				prev_shift_ticks = fuck_tiks;
				double_tapped = 0;
			}
			else {
				double_tapped++;
				prev_shift_ticks = 0;
			}
			if (prev_shift_ticks > 0)
			{
				if (CMAntiAim::Get().can_exploit(prev_shift_ticks))
				{
					CMAntiAim::Get().shot_dt = true;
					if (csgo->cmd->buttons & IN_ATTACK)
					{
						csgo->m_nTickbaseShift = prev_shift_ticks;
						CMAntiAim::Get().did_shot = true;
						reset = false;
						CMAntiAim::Get().cmd_tick = csgo->cmd->command_number;
						CMAntiAim::Get().base_tick = csgo->local->GetTickBase();
					}
					else
					{
						if ((!(csgo->cmd->buttons & IN_ATTACK) || !Ragebot::Get().shot) && CMAntiAim::Get().did_shot) {
							csgo->dt_charged = false;
							csgo->skip_ticks = vars.ragebot.dt_instant ? 17 : 8;
							CMAntiAim::Get().did_shot = false;
						}
					}

				}
				else
				{
					CMAntiAim::Get().shot_dt = false;
					process_ticks = 0;
				}
			}
			did_shift_before = prev_shift_ticks != 0;

		}
		else {
			csgo->bShootedFirstBullet = false;
			csgo->TickShifted = false;
			csgo->nSinceUse = 0;
		}
		if (vars.ragebot.hide_shots->active && !vars.ragebot.double_tap->active)
		{
			prev_shift_ticks = 0;

			auto can_shift_shot = CMAntiAim::Get().can_exploit(7);
			auto can_shot = CMAntiAim::Get().can_exploit(abs(-1 - prev_shift_ticks));

			if (can_shift_shot || !can_shot && !did_shift_before)
			{
				prev_shift_ticks = 7;
				double_tapped = 0;
			}
			else {
				double_tapped++;
				prev_shift_ticks = 0;
			}
			if (prev_shift_ticks > 0)
			{
				if (CMAntiAim::Get().can_exploit(7))
				{

					CMAntiAim::Get().shot_dt = true;
					if (csgo->cmd->buttons & IN_ATTACK)
					{
						csgo->m_nTickbaseShift = 7;
						CMAntiAim::Get().did_shot = true;
						reset = false;
						CMAntiAim::Get().cmd_tick = csgo->cmd->command_number;
						CMAntiAim::Get().base_tick = csgo->local->GetTickBase();
					}
				}
				else
				{
					CMAntiAim::Get().shot_dt = false;
					process_ticks = 0;
				}
			}
			did_shift_before = prev_shift_ticks != 0;
		}
		else {
			csgo->bShootedFirstBullet = false;
			csgo->TickShifted = false;
			csgo->nSinceUse = 0;
		}
	}
}

bool __stdcall Hooked_CreateMove(float a, CUserCmd* cmd) {
	static auto CreateMove = g_pClientModeHook->GetOriginal< CreateMoveFn >(24);
	csgo->updatelocalplayer();
	Ragebot::Get().DropTarget();
	if (!cmd || !cmd->command_number || !csgo->local || csgo->DoUnload)
		return CreateMove(interfaces.client_mode, a, cmd);

	csgo->cmd = cmd;
	in_cm = false;
	csgo->original = cmd->viewangles;

	PVOID pebp;
	__asm mov pebp, ebp;
	bool* pbSendPacket = (bool*)(*(PDWORD)pebp - 0x1C);
	bool& bSendPacket = *pbSendPacket;

	csgo->send_packet = true;

	DoClantag();

	if (csgo->local->isAlive())
	{
		if (csgo->skip_ticks > 0 && !csgo->dt_charged) {
			csgo->skip_ticks--;
			csgo->skipped_ticks++;

			if (csgo->skip_ticks == 0) 
			{
				csgo->skipped_ticks = 0;
				csgo->dt_charged = true;
			}
			cmd->tick_count = INT_MAX;
			csgo->send_packet = true;
		}

		INetChannel* pNetChannel = (INetChannel*)csgo->client_state->pNetChannel;
		csgo->tick_rate = 1.f / interfaces.global_vars->interval_per_tick;

		csgo->vecUnpredictedVel = csgo->local->GetVelocity();

		in_cm = true;
		csgo->unpdred_tick = interfaces.global_vars->tickcount;
		Ragebot::Get().DropTarget();

		if (vars.misc.bunnyhop)
			features->Bunnyhop->Run(csgo->original);

		csgo->weapon = csgo->local->GetWeapon();

		csgo->origin = csgo->local->GetRenderOrigin();

		csgo->unpred_eyepos = csgo->local->GetEyePosition();

		if (csgo->weapon)
		{
			csgo->g_bOverrideVelMod = true;

			if (csgo->g_flVelMod < 1.f)
				*(bool*)((uintptr_t)interfaces.prediction + 0x24) = true;

			csgo->weapon_struct.next_attack = csgo->weapon->NextSecondaryAttack();

			CMAntiAim::Get().Fakelag(csgo->send_packet);

			CEnginePrediction::Get().Start(cmd, csgo->local);
			{
				csgo->weapon_data = csgo->weapon->GetCSWpnData();

				if (vars.misc.knifebot)
					CKnifebot::Get().Run();

				if (vars.ragebot.enable)
					Ragebot::Get().Run();

				csgo->eyepos = csgo->local->GetEyePosition();

				ProcessMissedShots();
				if (vars.antiaim.enable)
				{
					FakeDuck(bSendPacket);
					CMAntiAim::Get().Run(csgo->send_packet);
					if (vars.antiaim.enable) {
						CMAntiAim::Get().Sidemove();
					}
				}

				if (csgo->cmd->buttons & IN_ATTACK)
				{
					if (auto net = interfaces.engine->GetNetChannelInfo(); net != nullptr)
						csgo->impact_time = interfaces.global_vars->curtime + net->GetLatency(MAX_FLOWS);
				}

				csgo->g_bOverrideVelMod = false;

				if (csgo->client_state->iChokedCommands <= 3) {
					InitDoubleTap();
				}

				if (vars.ragebot.enable && vars.ragebot.double_tap->active && !csgo->game_rules->IsFreezeTime())
				{
					if (!csgo->disable_dt)
					{
						if (!CanDT())
							csgo->dt_charged = false;
						else if (!csgo->dt_charged && csgo->skip_ticks == 0) {
							csgo->skip_ticks = vars.ragebot.dt_instant ? 17 : 12;
						}
					}
				}

				MoveFixer::Run(csgo->original);
			}
			CEnginePrediction::Get().Finish(csgo->local);

			CGrenadePrediction::Get().Tick(csgo->cmd->buttons);
		}

		c_js_loader::execute_scripts(create_move_hk);

		csgo->cmd->viewangles = Math::normalize(csgo->cmd->viewangles);

		csgo->choke_meme = csgo->local->GetSimulationTime() != csgo->local->GetOldSimulationTime();

		if (csgo->send_packet) {
			csgo->FakeAngle = csgo->cmd->viewangles;
		}
		else {
			csgo->VisualAngle = csgo->cmd->viewangles;
		}
		resolver->StoreAntifreestand();
		
		if (Ragebot::Get().IsAbleToShoot() && csgo->cmd->buttons & IN_ATTACK)
			csgo->last_shot_angle = csgo->cmd->viewangles;
		
		static INetChannel* old_net_chan = nullptr;
		if (csgo->local->isAlive()) {
			auto animstate = csgo->local->GetPlayerAnimState();
			if (animstate)
				csgo->desync_angle = animstate->m_flGoalFeetYaw;
		}
	}
	else {
		csgo->ForceOffAA = false;
		csgo->send_packet = true;
		in_cm = false;
	}

	cmd = csgo->cmd;
	bSendPacket = csgo->send_packet;

	return false;
}


int __stdcall Hooked_Camera(ConVar* var, void* return_address, void* eax)
{
	if (csgo->DoUnload)
		return 0;

	static auto return_to_modify_eye_pos = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::modify_eye_pos::s().c_str());

	const auto local = csgo->local;
	bool on_ground = false;
	auto animstate = local->GetPlayerAnimState();
	if (animstate)
		on_ground = animstate->m_bInHitGroundAnimation;
	if (return_address == (void*)return_to_modify_eye_pos)
		return !in_cm || (local && !on_ground);

	return 0;
}

// ReSharper disable once CppDeclaratorNeverUsed
static uint32_t get_bool_retn_address = 0;

__declspec(naked) bool get_bool(void* ecx, void* edx)
{
	_asm
	{
		push eax

		mov eax, [esp + 4]
		mov get_bool_retn_address, eax

		push get_bool_retn_address
		push ecx
		call Hooked_Camera

		cmp eax, 0
		je _retn0

		cmp eax, 1
		je _retn1

		cmp eax, 2
		je _retn2

		_retn0 :
		mov al, 0
			retn

			_retn1 :
		pop eax

			mov eax, get_bool_retn_address
			add eax, 0x462

			push eax

			retn

			_retn2 :
		pop eax

			mov eax, get_bool_retn_address
			add eax, 0x462

			push eax

			retn
	}
}