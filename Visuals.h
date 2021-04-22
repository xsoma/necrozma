#pragma once
#include "Hooks.h"
#include "RageBackTracking.h"
#include "Hitmarker.h"
struct BOX
{
	int h, w, x, y;
};

class CVisuals
{
public:
	std::vector<IBasePlayer*> players;
	void DrawLocalShit(IDirect3DDevice9* pDevice);
	void Draw();
	//void DrawWeapon(IDirect3DDevice9* pDevice);
	void RecordInfo();
	void NightMode();
	void DrawAngleLine(Vector origin, float angle, color_t color);
	void OnDormant();
	void AdjustDormant(IBasePlayer* player, SndInfo_t& sound);
	void ModelChanger();
	void Spectatorlist();
	void Penetration_crosshair();
	bool IsValidSound(SndInfo_t& sound);
	Vector strored_origin[64];

	struct SoundPlayer
	{
		void reset()
		{
			m_iIndex = 0;
			m_iReceiveTime = 0;
			m_vecOrigin = Vector(0, 0, 0);
			/* Restore data */
			m_nFlags = 0;
			player = nullptr;
			m_vecAbsOrigin = Vector(0, 0, 0);
			m_bDormant = false;
		}

		void Override(SndInfo_t& sound) {
			m_iIndex = sound.m_nSoundSource;
			m_vecOrigin = *sound.m_pOrigin;
			m_iReceiveTime = interfaces.global_vars->realtime;
		}

		int m_iIndex = 0;
		float m_iReceiveTime = 0;
		Vector m_vecOrigin = Vector(0, 0, 0);
		/* Restore data */
		int m_nFlags = 0;
		IBasePlayer* player = nullptr;
		Vector m_vecAbsOrigin = Vector(0, 0, 0);
		bool m_bDormant = false;
	} m_cSoundPlayers[64];
	CUtlVector<SndInfo_t> m_utlvecSoundBuffer;
	CUtlVector<SndInfo_t> m_utlCurSoundList;
	std::vector<SoundPlayer> m_arRestorePlayers;
};
enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class CGrenadePrediction : public Singleton<CGrenadePrediction>
{
private:
	std::vector<Vector> path;

	int type = 0;
	int act = 0;

	void Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles);
	void Simulate(CViewSetup* setup);

	int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval);
	bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval);

	void TraceHull(Vector& src, Vector& end, trace_t& tr);
	void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
	void PushEntity(Vector& src, const Vector& move, trace_t& tr);
	void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval);
	int PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
public:
	void Tick(int buttons);
	void View(CViewSetup* setup);
	void Paint();
};
