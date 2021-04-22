#pragma once
#include "Hooks.h"

void util_trace_hull(Vector vec_start, Vector vec_end, const unsigned int n_mask, Vector extents, trace_t* p_trace)
{
	CTraceFilterNoPlayers filter;

	Ray_t ray;
	ray.Init(vec_start, vec_end);
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	interfaces.trace->TraceRay(ray, n_mask, &filter, p_trace);
}

void Thirdperson()
{
	if (!interfaces.engine->IsInGame() || !csgo->local)
		return;

	if (!vars.misc.thirdperson)
	{
		enabledtp = false;
		return;
	}
	static Vector view_angles;
	interfaces.engine->GetViewAngles(view_angles);

	if (csgo->local->isAlive() && enabledtp)
	{
		auto animstate = csgo->local->GetPlayerAnimState();

		static auto was_inlandanim = false;

		interfaces.input->m_fCameraInThirdPerson = true;
		interfaces.input->m_vecCameraOffset = Vector(view_angles.x, view_angles.y, vars.visuals.thirdperson_dist);

		const auto cam_hull_offset = 12.f + (vars.misc.worldfov / 4.8f - 18.f);
		const Vector cam_hull_extents(cam_hull_offset, cam_hull_offset, cam_hull_offset);
		Vector cam_forward;
		auto origin = csgo->local->GetEyePosition();

		Math::AngleVectors(Vector(interfaces.input->m_vecCameraOffset.x, interfaces.input->m_vecCameraOffset.y, 0.0f), &cam_forward);

		trace_t trace;
		util_trace_hull(origin, origin - (cam_forward * interfaces.input->m_vecCameraOffset.z), MASK_SOLID & ~CONTENTS_MONSTER, cam_hull_extents, &trace);

		interfaces.input->m_vecCameraOffset.z *= trace.fraction;
	}
	else if (interfaces.input->m_fCameraInThirdPerson)
	{
		interfaces.input->m_fCameraInThirdPerson = false;
	}

	static auto b_once = false;

	if (csgo->local->isAlive())
	{
		b_once = false;
		return;
	}

	if (b_once)
		csgo->local->SetObserverMode(5);

	if (csgo->local->GetObserverMode() == 4)
		b_once = true;
}

void __stdcall Hooked_OverrideView(void* pSetup)
{
	static auto OverRideView = g_pClientModeHook->GetOriginal< OverRideViewFn >(18);

	if (csgo->DoUnload)
		return 	OverRideView(pSetup);

	auto Setup = reinterpret_cast<CViewSetup*>(pSetup);

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && !csgo->local->IsScoped())
	{
		Setup->fov = vars.misc.worldfov;
	}

	Thirdperson();

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected())
	{
		if (csgo->local && csgo->local->isAlive()) {

			if (vars.visuals.remove & 1)
				Setup->angles -= csgo->local->GetPunchAngle() * 0.9f + csgo->local->GetViewPunchAngle();
			if (vars.visuals.remove & 16)
			{
				if (csgo->weapon && csgo->weapon->isSniper() && csgo->local->IsScoped())
				{
					switch (csgo->weapon->GetZoomLevel())
					{
					case 1:
					{
						if (vars.misc.worldfov > 90)
							Setup->fov = vars.misc.worldfov;
						else
							Setup->fov = 90;
					}
					break;
					case 2:
					{
						if (vars.misc.worldfov > 90)
							Setup->fov = vars.misc.worldfov / 2;
						else
							Setup->fov = 45;
					}
					break;
					}


				}
			}
			if (csgo->fake_duck)
				Setup->origin.z = csgo->local->GetAbsOrigin().z + 64.f;

			CGrenadePrediction::Get().View(Setup);
		}

	}

	OverRideView(pSetup);
}

float __stdcall Hooked_ViewModel()
{
	float fov = g_pClientModeHook->GetOriginal<GetViewModelFOVFn>(35)();

	if (csgo->DoUnload)
		return fov;

	if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame())
	{
		if (csgo->local)
		{
			fov = vars.misc.viewmodelfov == 0 ? 68 : vars.misc.viewmodelfov;
		}
	}
	return fov;
}
