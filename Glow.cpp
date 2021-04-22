#include "Glow.h"

bool IsProjectile(IBasePlayer* m_entity)
{
	if (m_entity->GetClientClass()->m_ClassID == ClassId->CSmokeGrenadeProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CSensorGrenadeProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CMolotovProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CDecoyProjectile ||
		m_entity->GetClientClass()->m_ClassID == ClassId->CBaseCSGrenadeProjectile
		)
		return true;
	else
		return false;
}

void GlowESP::Draw()
{
	static CGlowObjectManager::GlowObjectDefinition_t* glow_object;
	for (auto i = 0; i < interfaces.glow_manager->size; i++)
	{
		glow_object = &interfaces.glow_manager->m_GlowObjectDefinitions[i];

		IBasePlayer *m_entity = glow_object->m_pEntity;

		if (!glow_object->m_pEntity)
			continue;

		if (m_entity->GetClientClass()->m_ClassID == ClassId->CCSPlayer)
		{
			if (!m_entity->isAlive() || m_entity->IsDormant())
				continue;
			glow_object->m_bFullBloomRender = false;
			if (m_entity == csgo->local)
			{
				if (vars.visuals.local_glow && interfaces.input->m_fCameraInThirdPerson)
				{

					glow_object->m_vGlowcolor_t = Vector(
						vars.visuals.local_glow_clr[0] / 255.f,
						vars.visuals.local_glow_clr[1] / 255.f,
						vars.visuals.local_glow_clr[2] / 255.f
					);
					if (vars.visuals.glowtype == 1)
						glow_object->m_bPulsatingChams = true;
					glow_object->m_flGlowAlpha = vars.visuals.local_glow_clr[3] / 255.f;
					glow_object->m_bRenderWhenOccluded = true;
					glow_object->m_bRenderWhenUnoccluded = false;
				}
				else
					continue;
			}
			
			if (m_entity->GetTeam() == csgo->local->GetTeam())
				continue;

			if (vars.visuals.glow)
			{
				glow_object->m_vGlowcolor_t = Vector(
					vars.visuals.glow_color[0] / 255.f,
					vars.visuals.glow_color[1] / 255.f,
					vars.visuals.glow_color[2] / 255.f
				);
				if (vars.visuals.glowtype == 1)
					glow_object->m_bPulsatingChams = true;
				glow_object->m_flGlowAlpha = vars.visuals.glow_color[3] / 255.f;
				glow_object->m_bRenderWhenOccluded = true;
				glow_object->m_bRenderWhenUnoccluded = false;
			}
		}
		if (m_entity->GetClientClass()->m_ClassID == ClassId->CC4 && m_entity->GetClientClass()->m_ClassID == ClassId->CPlantedC4 && vars.visuals.glow_bomb)
		{
			glow_object->m_vGlowcolor_t = Vector(
				vars.visuals.glow_bomb_color[0] / 255.f,
				vars.visuals.glow_bomb_color[1] / 255.f,
				vars.visuals.glow_bomb_color[2] / 255.f
			);
			if (vars.visuals.glowtype == 1)
				glow_object->m_bPulsatingChams = true;
			glow_object->m_flGlowAlpha = vars.visuals.glow_bomb_color[3] / 255.f;
			glow_object->m_bRenderWhenOccluded = true;
			glow_object->m_bRenderWhenUnoccluded = false;
		}
		if (IsProjectile(m_entity) && vars.visuals.glow_grenades)
		{
			glow_object->m_vGlowcolor_t = Vector(
				vars.visuals.glow_grenades_color[0] / 255.f,
				vars.visuals.glow_grenades_color[1] / 255.f,
				vars.visuals.glow_grenades_color[2] / 255.f
			);
			if (vars.visuals.glowtype == 1)
				glow_object->m_bPulsatingChams = true;
			glow_object->m_flGlowAlpha = vars.visuals.glow_color[3] / 255.f;
			glow_object->m_bRenderWhenOccluded = true;
			glow_object->m_bRenderWhenUnoccluded = false;
		}
	}
}

GlowESP* glow = new GlowESP();