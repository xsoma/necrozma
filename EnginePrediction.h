#pragma once
#include "hooks.h"

class CEnginePrediction : public Singleton<CEnginePrediction>
{
public:
	void Start(CUserCmd* cmd, IBasePlayer* local);
	void Finish(IBasePlayer* local);

	int m_prediction_player;
	int m_prediction_seed;
private:
	CMoveData data;
	struct {
		float curtime, frametime;
		int tickcount, tickbase;
	}old_vars;
};