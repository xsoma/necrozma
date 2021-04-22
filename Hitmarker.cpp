#include "Hitmarker.h"
CHitMarker* g_Hitmarker = new CHitMarker();
IBasePlayer* CHitMarker::GetPlayer(int ID)
{
	int index = interfaces.engine->GetPlayerForUserID(ID);
	return interfaces.ent_list->GetClientEntity(index);
}
void CHitMarker::Paint()
{
	if (!vars.visuals.hitmarker)
		return;
	if (hitmarkers.size() == 0)
		return;

	// draw
	for (int i = 0; i < hitmarkers.size(); i++)
	{
		Vector pos3D = Vector(hitmarkers[i].impact.x, hitmarkers[i].impact.y, hitmarkers[i].impact.z), pos2D;
		if (!Math::WorldToScreen(pos3D, pos2D))
			continue;

		auto r = vars.visuals.hitmarker_color[0],
			g = vars.visuals.hitmarker_color[1],
			b = vars.visuals.hitmarker_color[2];

		int w, h = 0;

		interfaces.engine->GetScreenSize(w, h);


		if (vars.visuals.hitmarkers & 1)
		{
			g_Render->DrawLine(pos2D.x + 4, pos2D.y + 4, pos2D.x + 8, pos2D.y + 8, color_t(r, g, b, hitmarkers[i].alpha));
			g_Render->DrawLine(pos2D.x - 4, pos2D.y - 4, pos2D.x - 8, pos2D.y - 8, color_t(r, g, b, hitmarkers[i].alpha));
			g_Render->DrawLine(pos2D.x + 4, pos2D.y - 4, pos2D.x + 8, pos2D.y - 8, color_t(r, g, b, hitmarkers[i].alpha));
			g_Render->DrawLine(pos2D.x - 4, pos2D.y + 4, pos2D.x - 8, pos2D.y + 8, color_t(r, g, b, hitmarkers[i].alpha));
		}
		if (vars.visuals.hitmarkers & 2)
		{
			g_Render->DrawLine(w / 2 + 4, h / 2 + 4, w / 2 + 8, h / 2 + 8, color_t(r, g, b, hitmarkers[i].alpha));
			g_Render->DrawLine(w / 2 - 4, h / 2 - 4, w / 2 - 8, h / 2 - 8, color_t(r, g, b, hitmarkers[i].alpha));
			g_Render->DrawLine(w / 2 + 4, h / 2 - 4, w / 2 + 8, h / 2 - 8, color_t(r, g, b, hitmarkers[i].alpha));
			g_Render->DrawLine(w / 2 - 4, h / 2 + 4, w / 2 - 8, h / 2 + 8, color_t(r, g, b, hitmarkers[i].alpha));
		}
	}
	// proceeed
	for (int i = 0; i < hitmarkers.size(); i++) {

		if (hitmarkers[i].time + 1.25f <= csgo->get_absolute_time()) {
			hitmarkers[i].alpha -= 1;
		}

		if (hitmarkers[i].alpha <= 0)
			hitmarkers.erase(hitmarkers.begin() + i);
	}
}
void CHitMarker::OnPlayerHurt(IGameEvent* pEvent)
{
	auto attacker = GetPlayer(pEvent->GetInt("attacker"));
	auto victim = GetPlayer(pEvent->GetInt("userid"));

	if (!attacker || !victim || attacker != csgo->local)
		return;

	Vector enemypos = victim->GetOrigin();
	impact_info best_impact;
	float best_impact_distance = -1;
	float time = csgo->get_absolute_time();


	for (int i = 0; i < impacts.size(); i++)
	{
		auto iter = impacts[i];
		if (time > iter.time + 1.f)
		{
			impacts.erase(impacts.begin() + i);
			continue;
		}
		Vector position = Vector(iter.x, iter.y, iter.z);
		float distance = position.DistTo(enemypos);
		if (distance < best_impact_distance || best_impact_distance == -1)
		{
			best_impact_distance = distance;
			best_impact = iter;
		}
	}
	if (best_impact_distance == -1)
		return;

	hitmarker_info info;
	info.impact = best_impact;
	info.alpha = 255;
	info.time = csgo->get_absolute_time();
	hitmarkers.push_back(info);
}
void CHitMarker::OnBulletImpact(IGameEvent* pEvent)
{
	auto shooter = GetPlayer(pEvent->GetInt("userid"));
	if (!shooter || shooter != csgo->local)
		return;
	impact_info info;
	info.x = pEvent->GetFloat("x");
	info.y = pEvent->GetFloat("y");
	info.z = pEvent->GetFloat("z");
	info.time = csgo->get_absolute_time();
	impacts.push_back(info);
}


