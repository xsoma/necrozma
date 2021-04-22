#include "Knifebot.h"
#include "Ragebot.h"

void CKnifebot::Run()
{
	auto can_shoot = []() {
		auto weapon = csgo->weapon;
		if (!csgo->local || !csgo->weapon || csgo->weapon->InReload() || csgo->weapon->GetAmmo(false) == 0)
			return false;

		if (csgo->cmd->weaponselect || csgo->local->GetFlags() & FL_FROZEN || csgo->game_rules->IsFreezeTime())
			return false;

		return (interfaces.global_vars->curtime >= weapon->NextPrimaryAttack()
			|| interfaces.global_vars->curtime >= weapon->NextSecondaryAttack());
	};

	if (!csgo->weapon->IsKnife())
		return;
	if (!can_shoot())
		return;
	IBasePlayer* best_entity = nullptr;
	float best_distance = 99999.f;
	for (auto i = 0; i <= interfaces.global_vars->maxClients; i++)
	{
		auto pEntity = interfaces.ent_list->GetClientEntity(i);
		if (pEntity == nullptr)
			continue;
		if (!pEntity->IsValid())
			continue;

		float dist = (csgo->local->GetAbsOrigin() - pEntity->GetAbsOrigin()).Length2D();
		if (best_distance >= dist)
		{
			best_entity = pEntity;
			best_distance = dist;
		}
	}

	if (best_entity != nullptr)
	{

		auto record = g_Animfix->get_latest_animation(best_entity);
		if (!record.has_value() || !record.value()->player)
			return;
		Vector stomach = Ragebot::Get().GetPoint(record.value()->player, (int)CSGOHitboxID::Stomach, record.value()->bones);
		if (stomach == Vector(0, 0, 0))
			return;

		Vector AimAngle = Math::CalculateAngle(csgo->local->GetEyePosition(), stomach);
		if (best_entity->GetHealth() <= 35 && best_distance < 65) {
			csgo->cmd->buttons |= IN_ATTACK;
			csgo->cmd->viewangles = Math::CalculateAngle(csgo->local->GetEyePosition(), best_entity->GetEyePosition());
		}
		else {
			if (Math::IsNearEqual(best_entity->GetEyeAngles().y, Math::CalculateAngle(csgo->local->GetOrigin(), best_entity->GetEyeAngles()).y, 35.f))
			{
				if (best_distance < 50)
				{
					csgo->cmd->buttons |= IN_ATTACK2;
					csgo->cmd->viewangles = Math::CalculateAngle(csgo->local->GetEyePosition(), best_entity->GetEyePosition());
				}
			}
			else
			{
				if (best_distance < 50)
					csgo->cmd->buttons |= IN_ATTACK2;
				else if (best_distance < 65)
					csgo->cmd->buttons |= IN_ATTACK;

				if (csgo->cmd->buttons & IN_ATTACK || csgo->cmd->buttons & IN_ATTACK2 && best_distance < 65)
				{
					csgo->cmd->viewangles = Math::CalculateAngle(csgo->local->GetEyePosition(), best_entity->GetOrigin());
				}
			}
		}
		csgo->cmd->tick_count = TIME_TO_TICKS(record.value()->sim_time + Ragebot::Get().LerpTime());
	}
}