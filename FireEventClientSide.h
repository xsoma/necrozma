#pragma once
#include "Hooks.h"
#include "Hitmarker.h"
enum round_gameflags
{
	ROUND_STARTING = 0,
	ROUND_IN_PROGRESS,
	ROUND_ENDING,
};
extern std::vector<IGameEventListener2*> g_pGameEventManager;



class CGameEventListener : public IGameEventListener2
{
private:
	std::string eventName;
	Fucntion Call;
	bool server;
public:
	CGameEventListener() : eventName(""), Call(nullptr), server(false)
	{
	}

	CGameEventListener(const std::string& gameEventName, const Fucntion Func, bool Server) : eventName(gameEventName), Call(Func), server(Server)
	{
		this->RegListener();
	}

	virtual void FireGameEvent(IGameEvent* event)
	{
		this->Call(event);
	}

	virtual int IndicateEventHandling(void)
	{
		return 0x2A;
	}

	int GetEventDebugID() override
	{
		return 0x2A;
	}

	void RegListener()
	{
		static bool first = true;
		if (!first)
			return;
		if (interfaces.event_manager->AddListener(this, this->eventName.c_str(), server))
			g_pGameEventManager.push_back(this);
	}
};

#define REG_EVENT_LISTENER(p, e, n, s) p = new CGameEventListener(n, e, s)


player_info_t GetInfo(int Index)
{
	player_info_t Info;
	interfaces.engine->GetPlayerInfo(Index, &Info);
	return Info;
}

std::string HitgroupToName(int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_GENERIC: return "generic";
	case HITGROUP_HEAD: return "head";
	case HITGROUP_CHEST: return "chest";
	case HITGROUP_STOMACH: return "stomach";
	case HITGROUP_LEFTARM: return "left arm";
	case HITGROUP_RIGHTARM: return "right arm";
	case HITGROUP_LEFTLEG: return "left leg";
	case HITGROUP_RIGHTLEG: return "right leg";
	case HITGROUP_GEAR: return "gear";
	default: return "?";
	}
}

void LogHits(IGameEvent* Event)
{
	if (!vars.visuals.eventlog)
		return;

	int attackerid = Event->GetInt("attacker");
	int entityid = interfaces.engine->GetPlayerForUserID(attackerid);
	if (entityid == interfaces.engine->GetLocalPlayer())
	{
		int nUserID = Event->GetInt("attacker");
		int nDead = Event->GetInt("userid");

		if (nUserID || nDead)
		{
			player_info_t killed_info = GetInfo(interfaces.engine->GetPlayerForUserID(nDead));
			player_info_t killer_info = GetInfo(interfaces.engine->GetPlayerForUserID(nUserID));
			string* Message = new string;
			*Message += "Hit ";
			*Message += killed_info.name;
			*Message += "'s ";
			int hitgroup = Event->GetInt("hitgroup");
			if (hitgroup != HITGROUP_GENERIC && hitgroup != HITGROUP_GEAR) {
				*Message += HitgroupToName(hitgroup);
			}
			*Message += " for ";
			*Message += Event->GetString("dmg_health");

			*Message += " (";
			*Message += Event->GetString("health");
			*Message += " health remaining) ";

			if (shot_snapshots.size() > 0)
			{
				auto snapshot = shot_snapshots.front();
				if (snapshot.entity == interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(nDead)))
				{
					*Message += snapshot.get_info(false);
					Ragebot::Get().last_hitted[snapshot.entity->GetIndex()] = snapshot;
					csgo->damagedshots++;
					shot_snapshots.erase(shot_snapshots.begin());
				}
			}
			Msg(*Message, color_t(124, 193, 21));
			delete Message;
		}
			
		
	}
}

IBasePlayer* GetPlayer(int ID)
{
	int index = interfaces.engine->GetPlayerForUserID(ID);
	return interfaces.ent_list->GetClientEntity(index);
}

void ClearMissedShots(IGameEvent* game_event)
{
	if (!csgo->local || csgo->local->GetHealth() <= 0)
		return;

	for (int i = 1; i <= interfaces.global_vars->maxClients; i++)
	{
		auto Entity = interfaces.ent_list->GetClientEntity(i);
		if (Entity == nullptr)
			continue;
		csgo->missedshots[Entity->EntIndex()] = 0;
		csgo->firedshots[Entity->EntIndex()] = 0;
	}


}

void player_hurt(IGameEvent* game_event) {

	LogHits(game_event);
	int attackerid = game_event->GetInt("attacker");
	int entityid = interfaces.engine->GetPlayerForUserID(attackerid);
	if (entityid == interfaces.engine->GetLocalPlayer())
	{
		if (vars.visuals.hitmarker_sound)
			interfaces.engine->ClientCmd_Unrestricted("play buttons/arena_switch_press_02.wav", 0);
		//interfaces.engine->ClientCmd_Unrestricted("play resource/warning.wav", 0);
	}
	g_Hitmarker->OnPlayerHurt(game_event);

	if (!csgo->local || csgo->local->GetHealth() <= 0 || !game_event)
		return;

	if (shot_snapshots.size() <= 0)
		return;

	auto& snapshot = shot_snapshots.front();

	if (interfaces.engine->GetPlayerForUserID(game_event->GetInt("attacker")) != interfaces.engine->GetLocalPlayer())
		return;

	if (!snapshot.bullet_impact)
	{
		snapshot.bullet_impact = true;
		snapshot.first_processed_time = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
	}
	snapshot.damage = game_event->GetInt("dmg_health");
	snapshot.hitgroup_hit = game_event->GetInt("hitgroup");
}

void bullet_impact(IGameEvent* game_event) {

	features->BulletTracer->Log(game_event);
	g_Hitmarker->OnBulletImpact(game_event);

	if (!csgo->local || csgo->local->GetHealth() <= 0 || !game_event)
		return;

	if (shot_snapshots.size() <= 0)
		return;

	auto& snapshot = shot_snapshots.front();

	if (interfaces.engine->GetPlayerForUserID(game_event->GetInt("userid")) != interfaces.engine->GetLocalPlayer())
		return;

	if (!snapshot.bullet_impact)
	{
		snapshot.bullet_impact = true;
		snapshot.first_processed_time = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
		snapshot.impact = Vector(game_event->GetFloat("x"), game_event->GetFloat("y"), game_event->GetFloat("z"));
	}


}

void weapon_fire(IGameEvent* game_event) {
	if (!csgo->local || csgo->local->GetHealth() <= 0 || !game_event)
		return;
	csgo->last_shoot_pos = csgo->local->GetEyePosition();
	if (shot_snapshots.size() <= 0)
		return;

	auto& snapshot = shot_snapshots.front();

	if (!snapshot.weapon_fire)
		snapshot.weapon_fire = true;
	csgo->not_damagedshots++;
}


inline static round_gameflags round_flags;
void player_death(IGameEvent* game_event) {
	int attacker = interfaces.engine->GetPlayerForUserID(game_event->GetInt("attacker"));
	int iLocalPlayer = interfaces.engine->GetLocalPlayer();
	if (vars.visuals.kill_effect && csgo->local && csgo->local->isAlive() && attacker == iLocalPlayer)
		csgo->local->HealthShotBoostExpirationTime() = interfaces.global_vars->curtime + 1.f;
}

void round_end(IGameEvent* game_event) {
	csgo->ForceOffAA = true;
	g_Hitmarker->hitmarkers.clear();
	features->Visuals->m_utlCurSoundList.RemoveAll();
	features->Visuals->m_utlvecSoundBuffer.RemoveAll();
	ClearMissedShots(game_event);
}

void DoBuyBot() {
	if (vars.misc.autobuy.enable) {

		if (vars.misc.autobuy.main == 0) {
			interfaces.engine->ExecuteClientCmd("buy scar20;");
			interfaces.engine->ExecuteClientCmd("buy g3sg1;");
		}
		else if (vars.misc.autobuy.main == 1) {
			interfaces.engine->ExecuteClientCmd("buy ssg08;");
		}
		else if (vars.misc.autobuy.main == 2) {
			interfaces.engine->ExecuteClientCmd("buy awp;");
		}

		if (vars.misc.autobuy.pistol == 0) {
			interfaces.engine->ExecuteClientCmd("buy elite;");
		}
		else if (vars.misc.autobuy.pistol == 1) {
			interfaces.engine->ExecuteClientCmd("buy deagle;");
			interfaces.engine->ExecuteClientCmd("buy revolver;");
		}

		if (vars.misc.autobuy.misc & 1) {
			interfaces.engine->ExecuteClientCmd("buy vesthelm;");
		}
		if (vars.misc.autobuy.misc & 2) {
			interfaces.engine->ExecuteClientCmd("buy vest;");
		}
		if (vars.misc.autobuy.misc & 4) {
			interfaces.engine->ExecuteClientCmd("buy hegrenade;");
		}
		if (vars.misc.autobuy.misc & 8) {
			interfaces.engine->ExecuteClientCmd("buy molotov;");
			interfaces.engine->ExecuteClientCmd("buy incgrenade;");
		}
		if (vars.misc.autobuy.misc & 32) {
			interfaces.engine->ExecuteClientCmd("buy smokegrenade;");
		}
		if (vars.misc.autobuy.misc & 64) {
			interfaces.engine->ExecuteClientCmd("buy taser;");
		}
		if (vars.misc.autobuy.misc & 128) {
			interfaces.engine->ExecuteClientCmd("buy defuser;");
		}
	}
}

void round_prestart(IGameEvent* game_event) {
	csgo->ForceOffAA = true;
	g_Hitmarker->hitmarkers.clear();
	features->Visuals->m_utlCurSoundList.RemoveAll();
	features->Visuals->m_utlvecSoundBuffer.RemoveAll();
	ClearMissedShots(game_event);
	csgo->disable_dt = true;
	CMAntiAim::Get().cmd_tick = 0;
	CMAntiAim::Get().base_tick = 0;
	csgo->m_nTickbaseShift = 0;
	DoBuyBot();
}
void round_freeze(IGameEvent* game_event) {
	
	round_flags = ROUND_IN_PROGRESS;
}
void round_start(IGameEvent* game_event)
{
	DoBuyBot();
	csgo->ForceOffAA = false;
	g_Hitmarker->hitmarkers.clear();
	ClearMissedShots(game_event);
	csgo->disable_dt = false;

	for (int i = 0; i < 64; i++) {
		features->Visuals->strored_origin[i] = Vector();
		csgo->EntityAlpha[i] = 0.f;
		csgo->LastSeenTime[i] = 0.f;
	}
	round_flags = ROUND_STARTING;
}

CGameEventListener* _player_hurt;
CGameEventListener* _bullet_impacts;
CGameEventListener* _weapon_fire;
CGameEventListener* _player_death;
CGameEventListener* _round_end;
CGameEventListener* _round_prestart;
CGameEventListener* _round_start;
CGameEventListener* _round_freeze;

void RegListeners()
{
	REG_EVENT_LISTENER(_player_hurt, &player_hurt, "player_hurt", false);
	REG_EVENT_LISTENER(_bullet_impacts, &bullet_impact, "bullet_impact", false);
	REG_EVENT_LISTENER(_weapon_fire, &weapon_fire, "weapon_fire", false);
	REG_EVENT_LISTENER(_player_death, &player_death, "player_death", false);
	REG_EVENT_LISTENER(_round_end, &round_end, "round_end", false);
	REG_EVENT_LISTENER(_round_prestart, &round_prestart, "round_prestart", false);
	REG_EVENT_LISTENER(_round_start, &round_start, "round_start", false);
	REG_EVENT_LISTENER(_round_freeze, &round_freeze, "round_freeze_end", false);
}